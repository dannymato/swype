#pragma once
#include "wl-roots-includes.h"
#include "cursor.h"
#include "event_handler.h"
#include "keyboard.h"
#include "util.h"
#include "view.h"

class SeatRequestSetCursorEvent
	: public wlr_seat_pointer_request_set_cursor_event {};

class SeatRequestSelectionEvent
	: public wlr_seat_request_set_selection_event {};

class Seat {
public:
	Seat(wlr_seat* seat);

	Ref<EventHandler<SeatRequestSetCursorEvent>> requestCursor() { return _requestCursor; }
	Ref<EventHandler<SeatRequestSelectionEvent>> requestSelection() { return _requestSelection; }

	void setCapabilies(uint32_t capabilties);
	void setKeyboard(const Keyboard& keyboard);
	void notifyKey(uint32_t timeMSec, uint32_t key, uint32_t state);
	void notifyModifiers(wlr_keyboard_modifiers* modifiers);
	void notifyPointerFrame();
	void notifyEnter(wlr_surface* surface, double sx, double sy);
	void notifyMotion(uint32_t time, double sx, double sy);
	void clearPointerFocus();
	void notifyButton(uint32_t timeMSec, uint32_t button, wlr_button_state state);
	void notifyAxis(PointerAxisEvent* event);
	void notifyAxis(uint32_t timeMSec, wlr_axis_orientation orientation, double value,
	int32_t discreteValue, wlr_axis_source source);
	void focusView(Ref<View> view, wlr_surface* surface);

private:

	wlr_seat* seat;
	Ref<EventHandler<SeatRequestSetCursorEvent>> _requestCursor;
	Ref<EventHandler<SeatRequestSelectionEvent>> _requestSelection;

};