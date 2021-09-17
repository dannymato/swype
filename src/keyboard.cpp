#include "keyboard.h"

Keyboard::Keyboard(wlr_input_device* device) : device(device) {
	xkb_context* context = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
	xkb_keymap* keymap = xkb_keymap_new_from_names(context, nullptr,
		XKB_KEYMAP_COMPILE_NO_FLAGS);

	wlr_keyboard_set_keymap(device->keyboard, keymap);
	xkb_keymap_unref(keymap);
	xkb_context_unref(context);
	wlr_keyboard_set_repeat_info(device->keyboard, 25, 600);

	_modifiers = CreateRef<EventHandler<void>>(&device->keyboard->events.modifiers);
	_key = CreateRef<EventHandler<wlr_event_keyboard_key>>(&device->keyboard->events.key);

}