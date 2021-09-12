#pragma once
#include "wl-roots-includes.h"
#include "display.h"
#include "output.h"
#include "xdg_shell.h"

#include <vector>
#include <string>

class Server {
public:
	Server();
	~Server();

	void run();

	friend class EventHelper;
private:
	void newOutputCallback(void* data);
	void outputRenderCallback(void* data);

	Display* display;
	std::unordered_map<OutputKey, Output, output_hash_fn> outputs;
	wl_event_loop* event_loop;
	wlr_backend* backend;
	wlr_renderer* renderer;
	wlr_output_layout* layout;
	XDGShell* shell;

	const std::string test = "Server String";

	wl_listener new_output;
	wl_listener output_frame;
};