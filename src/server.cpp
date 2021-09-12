#include "server.h"
#include "event_helper.h"
#include <iostream>

Server::Server() {
	display = new Display();
	this->event_loop = wl_display_get_event_loop(this->display->get());

	this->backend = wlr_backend_autocreate(this->display->get());
   //	this->backend = wlr_wl_backend_create(this->display->get(), "wayland-0");
	//wlr_wl_output_create(this->backend);

	this->renderer = wlr_backend_get_renderer(this->backend);
	wlr_renderer_init_wl_display(this->renderer, this->display->get());

	wlr_compositor_create(this->display->get(), this->renderer);
	wlr_data_device_manager_create(this->display->get());

	this->layout = wlr_output_layout_create();

	this->new_output.notify = 
		&EventHelper::memberFunction<Server, wl_listener, &Server::new_output, &Server::newOutputCallback>;
	wl_signal_add(&backend->events.new_output, &new_output);

	this->output_frame.notify =
		&EventHelper::memberFunction<Server, wl_listener, &Server::output_frame, &Server::outputRenderCallback>;

	this->output_destroyed.notify = 
		&EventHelper::memberFunction<Server, wl_listener, &Server::output_destroyed, &Server::outputDestroyedCallback>;

	shell = new XDGShell(display);

	cursor = Cursor(layout);
	cursor.addMotionListener([this](PointerMotionEvent* motion) {
		wlr_log(WLR_DEBUG, "Logging from the callback closure");
	});
}

Server::~Server() {
	std::cout << "Server destruct" << std::endl;
	if (shell) {
		delete shell;
	}
	if (display) {
		delete display;
	}
}

void Server::run() {
	auto socket = display->setSocket();
	std::cout << "Starting the server" << std::endl;
	if (!wlr_backend_start(this->backend)) {
		throw "Backend failed to start";
	}
	std::cout << "Started the backend" << std::endl;
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

	wl_signal_add(&new_output->events.frame, &this->output_frame);
	wl_signal_add(&new_output->events.destroy, &output_destroyed);

	outputs.emplace(OutputKey(new_output), new_output);
	wlr_log(WLR_DEBUG, "NEW OUTPUT\tOutputs Size: %ld", outputs.size());

	wlr_output_layout_add_auto(layout, new_output);
}

void Server::outputRenderCallback(void* data) {
	wlr_output* wl_output = (wlr_output*)data;

	Output rendering_out = nullptr;
	auto search = outputs.find(OutputKey(wl_output));
	if (search != outputs.end()) {
		rendering_out = search->second;
	} else {
		throw "Could not find Output";
	}

	timespec now;
	clock_gettime(CLOCK_MONOTONIC, &now);
	
	rendering_out.render(renderer, [this, now, wl_output] {
		const float color[4] = {0.3, 0.3, 0.3, 1.0};
		wlr_renderer_clear(renderer, color);

		shell->renderViews(now, wl_output, layout, renderer);
	});
}

void Server::outputDestroyedCallback(void* data) {
	wlr_log(WLR_DEBUG, "Output Destroyed");
	wlr_output* wl_output = (wlr_output*)data;
	// Drop the Output object so we don't leak the memory
	outputs.erase(OutputKey(wl_output));

	// Could be a bad time we'll have to see
	if (backend && wlr_backend_is_wl(backend) && outputs.empty()) {
		
	}
}
