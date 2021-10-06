#include "cursor.h"

#include "util.h"

Cursor::Cursor(wlr_output_layout* layout) {
	cursor = wlr_cursor_create();
	wlr_cursor_attach_output_layout(cursor, layout);

	xcursor_manager = wlr_xcursor_manager_create(nullptr, 24);
	wlr_xcursor_manager_load(xcursor_manager, 1);

	_motionHandler = CreateRef<EventHandler<PointerMotionEvent>>(&cursor->events.motion);
	_absoluteMotionHandler = CreateRef<EventHandler<AbsolutePointerMotionEvent>>(&cursor->events.motion_absolute);
	_buttonHandler = CreateRef<EventHandler<PointerButtonEvent>>(&cursor->events.button);
	_axisHandler = CreateRef<EventHandler<PointerAxisEvent>>(&cursor->events.axis);
}

void Cursor::addInputDevice(wlr_input_device* device) {
	wlr_cursor_attach_input_device(cursor, device);
}

void Cursor::setXCursorImage(const char* image) {
	wlr_xcursor_manager_set_cursor_image(xcursor_manager, image, cursor);
}

void Cursor::setXCursorImage(const std::string& image) {
	setXCursorImage(image.c_str());
}

void Cursor::move(wlr_input_device* device, const double x, const double y) {
	wlr_cursor_move(cursor, device, x, y);
}

void Cursor::moveAbsolute(wlr_input_device* device, const double x, const double y) {
	wlr_cursor_warp_absolute(cursor, device, x, y);
}

