#pragma once
#include "wl-roots-includes.h"
#include "event_handler.h"
#include <list>
#include <string>

class PointerMotionEvent : public wlr_event_pointer_motion {};
class AbsolutePointerMotionEvent :
	public wlr_event_pointer_motion_absolute {};

class PointerButtonEvent : public wlr_event_pointer_button {
public:
	bool isPressed() {
		return state == WLR_BUTTON_PRESSED;
	}

	bool isReleased() {
		return state == WLR_BUTTON_RELEASED;
	}

};
class PointerAxisEvent : public wlr_event_pointer_axis {};

class Cursor {
public:
	Cursor(wlr_output_layout* layout);

	Ref<EventHandler<PointerMotionEvent>> motionHandler() { return _motionHandler; }
	Ref<EventHandler<AbsolutePointerMotionEvent>> absoluteMotionHandler() { return _absoluteMotionHandler; }
	Ref<EventHandler<PointerButtonEvent>> buttonHandler() { return _buttonHandler; }
	Ref<EventHandler<PointerAxisEvent>> axisHandler() { return _axisHandler; }

	void addInputDevice(wlr_input_device* device);
	void setXCursorImage(const char* image);
	void setXCursorImage(const std::string& image);
	void move(wlr_input_device* device, const double x, const double y);
	void moveAbsolute(wlr_input_device* device, const double x, const double y);
	double x() const { return cursor->x; }
	double y() const { return cursor->y; }

	friend class EventHelper;
private:
	void motionCallback(void* data);
	wlr_cursor* cursor;
	wlr_xcursor_manager* xcursor_manager;
	wl_listener motion;
	Ref<EventHandler<PointerMotionEvent>> _motionHandler;
	Ref<EventHandler<AbsolutePointerMotionEvent>> _absoluteMotionHandler;
	Ref<EventHandler<PointerButtonEvent>> _buttonHandler;
	Ref<EventHandler<PointerAxisEvent>> _axisHandler;
};