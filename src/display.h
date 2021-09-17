#pragma once
#include "wl-roots-includes.h"
#include "seat.h"

class Display {
public:
	Display();

	~Display();

	wl_display* get();

	const char* setSocket();

	Seat createSeat();
	void destroyClients();

	void run();
	void stop();
private:
	wl_display* m_display;
	bool _shouldContinue;

};