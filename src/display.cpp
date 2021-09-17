#include "display.h"

#include <iostream>

Display::Display() {
	wlr_log(WLR_DEBUG, "Display Constructor");
	m_display = wl_display_create();
}

Display::~Display() {
	wlr_log(WLR_DEBUG, "Display Deconstructor");
	wl_display_destroy(m_display);
}

Seat Display::createSeat() {
	wlr_seat* seat = wlr_seat_create(m_display, "seat0");
	return Seat(seat);
}

wl_display* Display::get() {
	return m_display;
}

const char* Display::setSocket() {
	const char* socket = wl_display_add_socket_auto(m_display);
	if (!socket) {
		throw "Failed to create socket";
	}
	setenv("WAYLAND_DISPLAY", socket, true);
	return socket;
}

void Display::stop() { _shouldContinue = false; }

void Display::run() {
	_shouldContinue = true;
	wl_event_loop* loop = wl_display_get_event_loop(m_display);
	while (_shouldContinue) {

		wl_display_flush_clients(m_display);
		wl_event_loop_dispatch(loop, -1);
	}
}

void Display::destroyClients() {
	wl_display_destroy_clients(m_display);
}