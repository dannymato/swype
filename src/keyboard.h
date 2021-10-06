#pragma once

#include "wl-roots-includes.h"
#include "event_handler.h"
#include "util.h"

class Keyboard {
public:
	Keyboard(wlr_input_device* device);

	Ref<EventHandler<void>> modifiers() { return _modifiers; }
	Ref<EventHandler<wlr_event_keyboard_key>> key() { return _key; }
	wlr_input_device* getDevice() const { return device; }

private:
	wlr_input_device* device;
	Ref<EventHandler<void>> _modifiers;
	Ref<EventHandler<wlr_event_keyboard_key>> _key;
};