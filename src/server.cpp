#include "server.h"
#include "event_helper.h"
#include "command.h"
#include <iostream>
#include <functional>
#include <memory>

Server::Server() {
	display = new Display();
	this->event_loop = wl_display_get_event_loop(this->display->get());

	this->backend = wlr_backend_autocreate(this->display->get());
   //	this->backend = wlr_wl_backend_create(this->display->get(), "wayland-0");
	//wlr_wl_output_create(this->backend);
	if (!backend) {
		throw "Failed to create backend";
	}

	this->renderer = wlr_backend_get_renderer(this->backend);
	wlr_renderer_init_wl_display(this->renderer, this->display->get());

	wlr_compositor_create(this->display->get(), this->renderer);
	wlr_data_device_manager_create(this->display->get());

	this->layout = wlr_output_layout_create();

	this->new_output.notify =
		&EventHelper::memberFunction<Server, wl_listener, &Server::new_output, &Server::newOutputCallback>;
	wl_signal_add(&backend->events.new_output, &new_output);

	shell = std::make_unique<XDGShell>(display);
	cursor = CreateRef<Cursor>(layout);

	shell->surfaceRequestMove()->AddHandler([this] (Ref<View> view) {
		wlr_log(WLR_DEBUG, "Surface Requested Move");
		_mouseState = std::make_unique<MoveState>(view, *cursor);
	});

	shell->newSurfaceHandler()->addHandler([this](wlr_xdg_surface* surface) {
		if (surface->role != WLR_XDG_SURFACE_ROLE_TOPLEVEL)
			return;

		auto view = shell->topView();
		if (view) {
			seat->focusView(*view, surface->surface);
		}
	});

	cursor->motionHandler()->addHandler([this](PointerMotionEvent* motion) {
		cursor->move(motion->device, motion->delta_x, motion->delta_y);
		//wlr_log(WLR_DEBUG, "Relative Motion %lf.3, %lf.3", cursor->x(), cursor->y());
		handleMouseMovement(motion->time_msec);
	});

	cursor->absoluteMotionHandler()->addHandler([this](AbsolutePointerMotionEvent* event) {
		cursor->moveAbsolute(event->device, event->x, event->y);
		//wlr_log(WLR_DEBUG, "Absolute motion %lf.3, %lf.3", cursor->x(), cursor->y());
		handleMouseMovement(event->time_msec);
	});

	cursor->buttonHandler()->addHandler([this](PointerButtonEvent* event) {
		seat->notifyButton(event->time_msec, event->button, event->state);
		if (event->isPressed()) {
			double sx, sy;
			wlr_surface* surface;
			// Returns an optional
			auto view = shell->findViewAt(cursor, &surface, &sx, &sy);
			if (view) {
				seat->focusView(*view, surface);
				shell->moveViewToTop(*view);
			}
			else {
				shell->clearActivated();
			}
		} else if (event->isReleased()) {
			_mouseState = std::make_unique<PassthroughState>();
		}
	});

	cursor->axisHandler()->addHandler([this](PointerAxisEvent* event) {
		seat->notifyAxis(event);
	});

	_newInput = CreateRef<EventHandler<wlr_input_device>>(&backend->events.new_input);
	_newInput->addHandler(std::bind(&Server::newInputCallback, this, std::placeholders::_1));

	seat = std::make_unique<Seat>(display->createSeat());

	_mouseState = std::make_unique<PassthroughState>();
}

Server::~Server() {
	wlr_log(WLR_DEBUG, "Server Destruction");
	display->destroyClients();
	if (display) {
		delete display;
	}
}

void Server::run() {
	auto socket = display->setSocket();
	wlr_log(WLR_DEBUG, "Server startup");
	if (!wlr_backend_start(this->backend)) {
		throw "Backend failed to start";
	}
	wlr_log(WLR_DEBUG, "Backend started");
	wlr_log(WLR_INFO, "Running Wayland compositor on WAYLAND_DISPLAY=%s", socket);
	this->display->run();
}

void Server::newOutputCallback(void* data) {
	std::cout << "new output" << std::endl;
	wlr_output* new_output = (wlr_output*)data;
	if (!wl_list_empty(&new_output->modes)) {
			wlr_output_mode *mode = wlr_output_preferred_mode(new_output);
			wlr_output_set_mode(new_output, mode);
			wlr_output_enable(new_output, true);
			if (!wlr_output_commit(new_output)) {
					return;
			}
	}

	const auto &[iter, inserted] = outputs.emplace(OutputKey(new_output), new_output);
	auto& output = iter->second;
	if (!inserted) {
		throw "Output not inserted";
	}
	output.destroyed()->addHandler(std::bind(&Server::outputDestroyedCallback, this, std::placeholders::_1));
	output.frame()->addHandler(std::bind(&Server::outputRenderCallback, this, std::placeholders::_1));
	wlr_log(WLR_DEBUG, "NEW OUTPUT\tOutputs Size: %ld", outputs.size());

	wlr_output_layout_add_auto(layout, new_output);
}

void Server::newInputCallback(wlr_input_device* device) {
	switch(device->type) {
		case WLR_INPUT_DEVICE_KEYBOARD:
			newKeyboardCallback(device);
			break;
		case WLR_INPUT_DEVICE_POINTER:
			cursor->addInputDevice(device);
			break;
		default:
			break;
	}

	uint32_t caps = WL_SEAT_CAPABILITY_POINTER;
	if (!keyboards.empty()) {
		caps |= WL_SEAT_CAPABILITY_KEYBOARD;
	}

	seat->setCapabilies(caps);
}

void Server::newKeyboardCallback(wlr_input_device* device) {
	wlr_log(WLR_DEBUG, "New keyboard!");
	auto keyboard = keyboards.emplace_front(device);
	seat->setKeyboard(keyboard);
	keyboard.key()->addHandler(std::bind(&Server::handleKeyEvent, this, keyboard, std::placeholders::_1));
	keyboard.modifiers()->addHandler(
		std::bind(&Server::handleModifiers, this, keyboard, std::placeholders::_1)
	);
}

void Server::outputRenderCallback(wlr_output* wl_output) {

	auto search = outputs.find(OutputKey(wl_output));
	Output& rendering_out = search->second;

	timespec now;
	clock_gettime(CLOCK_MONOTONIC, &now);

	rendering_out.render(renderer, [this, now, wl_output] {
		shell->renderViews(now, wl_output, layout, renderer);
	});
}

void Server::outputDestroyedCallback(wlr_output* wl_output) {
	wlr_log(WLR_DEBUG, "Output Destroyed");
	// Drop the Output object so we don't leak the memory
	outputs.erase(OutputKey(wl_output));

	if (wlr_backend_is_wl(backend) && outputs.empty())
		display->stop();
}

void Server::handleKeyEvent(const Keyboard& keyboard, wlr_event_keyboard_key* event) {
	// libinput to xkbcommon keycodes
	uint32_t keycode = event->keycode + 8;

	const xkb_keysym_t *syms;
	int nsyms = xkb_state_key_get_syms(
		keyboard.getDevice()->keyboard->xkb_state, keycode, &syms
	);

	bool handled = false;
	uint32_t modifiers = wlr_keyboard_get_modifiers(keyboard.getDevice()->keyboard);
	if ((modifiers & WLR_MODIFIER_ALT) && event->state == WL_KEYBOARD_KEY_STATE_PRESSED) {
		for (int i = 0; i < nsyms; i++) {
			handled = handleKeybindings(syms[i]);
		}
	}

	if (!handled) {
		seat->setKeyboard(keyboard);
		seat->notifyKey(event->time_msec, event->keycode, event->state);
	}
}

bool Server::handleKeybindings(xkb_keysym_t sym) {
	switch (sym) {
	case XKB_KEY_Escape:
		display->stop();
		break;
	case XKB_KEY_Return:
		// Just for now but will hopefully by configurable
		Command::Exec("konsole");
		break;
	case XKB_KEY_d:
		Command::Exec("wofi", "--show", "drun");
		break;
	default:
		return false;
	}
	return true;
}

void Server::handleModifiers(const Keyboard& keyboard, [[maybe_unused]]void* nothing) {
	seat->notifyModifiers(&keyboard.getDevice()->keyboard->modifiers);
}

void Server::handleMouseMovement(uint32_t time) {
	switch (_mouseState->getType())
	{
	case Resize:
	case Move:
		// Don't want to pass through any other events
		_mouseState->update(*cursor);
		return;
	default:
		break;
	}
	double sx, sy;
	wlr_surface* surface;
	auto view = shell->findViewAt(cursor->x(), cursor->y(), &surface, &sx, &sy);
	if (!view) {
		cursor->setXCursorImage("left_ptr");
	}
	if (view && surface) {
		seat->notifyEnter(surface, sx, sy);
		seat->notifyMotion(time, sx, sy);
	} else {
		seat->clearPointerFocus();
	}
}
