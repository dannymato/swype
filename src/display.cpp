#include "display.h"

#include <iostream>

Display::Display() {
	std::cout << "Display constructed" << std::endl;
	m_display = wl_display_create();
}

Display::~Display() {
	std::cout << "Display desconstruct" << std::endl;
	wl_display_destroy_clients(m_display);
	wl_display_destroy(m_display);
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

void Display::run() {
	wl_display_run(m_display);
}