#include "cursor.h"
#include "util.h"

Cursor::Cursor(wlr_output_layout* layout) {
	cursor = wlr_cursor_create();
	wlr_cursor_attach_output_layout(cursor, layout);
	motion.notify = &EventHelper::memberFunction<Cursor, wl_listener, &Cursor::motion, &Cursor::motionCallback>;
	addMotionListener(&motion);
}

void Cursor::addMotionListener(wl_listener* listener) {
	wl_signal_add(&cursor->events.motion, listener);
}

void Cursor::addAbsoluteMotionListener(wl_listener* listener) {
	wl_signal_add(&cursor->events.motion_absolute, listener);
}

HandlerID Cursor::addMotionListener(MotionCallback callback) {
	HandlerID newID = UID::GenUID();
	motionCallbacks.emplace_back(std::make_pair(newID, callback));
	return newID;
}

// Returns true if the listener was found and removed
bool Cursor::removeMotionListener(HandlerID id) {
	auto initSize = motionCallbacks.size();
	motionCallbacks.remove_if([id](auto handler){ return id == handler.first; });
	return initSize != motionCallbacks.size();
}

void Cursor::motionCallback(void* data) {
	PointerMotionEvent* eventData = (PointerMotionEvent*)data;
	wlr_cursor_move(cursor, eventData->device,
		eventData->delta_x, eventData->delta_y);
	for (auto &[id, func] : motionCallbacks) {
		func(eventData);
	}
}

