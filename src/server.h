#pragma once
#include "wl-roots-includes.h"
#include "display.h"
#include "output.h"
#include "xdg_shell.h"
#include "cursor.h"
#include "keyboard.h"
#include "event_handler.h"

#include <forward_list>
#include <string>
#include <memory>

class Server {
public:
	Server();
	~Server();

	void run();

	friend class EventHandler<void>;
private:
	void newOutputCallback(void* data);
	void outputDestroyedCallback(wlr_output* wl_output);
	void outputRenderCallback(wlr_output* data);
	void newInputCallback(wlr_input_device* device);
	void newKeyboardCallback(wlr_input_device* device);
	void handleKeyEvent(const Keyboard& keyboard, wlr_event_keyboard_key* event);
	bool handleKeybindings(xkb_keysym_t sym);
	void handleModifiers(const Keyboard& keyboard, void* nothing);
	void handleMouseMovement(uint32_t time);

	Display* display;
	std::unordered_map<OutputKey, Output, output_hash_fn> outputs;
	wl_event_loop* event_loop;
	wlr_backend* backend;
	wlr_renderer* renderer;
	wlr_output_layout* layout;
	XDGShell* shell;
	Cursor* cursor;
	std::forward_list<Keyboard> keyboards;
	std::unique_ptr<Seat> seat;
	const std::string test = "Server String";

	Ref<EventHandler<wlr_input_device>> _newInput;

	wl_listener new_output;
};