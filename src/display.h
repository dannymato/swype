#pragma once
#include "wl-roots-includes.h"

class Display {
public:
	Display();
	
	~Display();

	wl_display* get();

	const char* setSocket();

	void run();
private:
	wl_display* m_display;
};