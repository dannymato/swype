#include "seat.h"

Seat::Seat(wlr_seat* seat) : seat(seat) {
	_requestCursor = CreateRef<EventHandler<SeatRequestSetCursorEvent>>(&seat->events.request_set_cursor);
	_requestSelection = CreateRef<EventHandler<SeatRequestSelectionEvent>>(&seat->events.request_set_selection);

}

void Seat::setCapabilies(uint32_t capabilities) {
	wlr_seat_set_capabilities(seat, capabilities);
}

void Seat::setKeyboard(const Keyboard& keyboard) {
	wlr_seat_set_keyboard(seat, keyboard.getDevice());
}

void Seat::notifyKey(uint32_t timeMSec, uint32_t key, uint32_t state) {
	wlr_seat_keyboard_notify_key(seat, timeMSec, key, state);
}

void Seat::notifyModifiers(wlr_keyboard_modifiers* modifiers) {
	wlr_seat_keyboard_notify_modifiers(seat, modifiers);
}

void Seat::notifyPointerFrame() {
	wlr_seat_pointer_notify_frame(seat);
}

void Seat::notifyEnter(wlr_surface* surface, double sx, double sy) {
	wlr_seat_pointer_notify_enter(seat, surface, sx, sy);
}

void Seat::notifyMotion(uint32_t time, double sx, double sy) {
	wlr_seat_pointer_notify_motion(seat, time, sx, sy);
}

void Seat::clearPointerFocus() {
	wlr_seat_pointer_clear_focus(seat);
}

void Seat::focusView(View& view, wlr_surface* surface) {
	wlr_surface* prev_surface = seat->keyboard_state.focused_surface;
	if (surface == prev_surface)
		return;

	if (prev_surface) {
		wlr_xdg_surface* previous = wlr_xdg_surface_from_wlr_surface(prev_surface);
		wlr_xdg_toplevel_set_activated(previous, false);
	}
	wlr_keyboard* keyboard = wlr_seat_get_keyboard(seat);

	view.setActivated();

	wlr_seat_keyboard_notify_enter(seat, view.getSurface(), keyboard->keycodes,
		keyboard->num_keycodes, &keyboard->modifiers);
}

void Seat::notifyButton(uint32_t timeMSec, uint32_t button, wlr_button_state state) {
	wlr_seat_pointer_notify_button(seat, timeMSec, button, state);
}