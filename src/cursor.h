#include "wl-roots-includes.h"
#include "event_helper.h"
#include <list>
#include <functional>

class PointerMotionEvent : public wlr_event_pointer_motion {

};

typedef uint64_t HandlerID;
typedef std::function<void(PointerMotionEvent*)> MotionCallback;

class Cursor {
public:
	Cursor() = default;
	Cursor(wlr_output_layout* layout);

	void addMotionListener(wl_listener* listener);
	HandlerID addMotionListener(MotionCallback callback);
	void addAbsoluteMotionListener(wl_listener* listener);
	void addButtonListener(wl_listener* listener);
	void addAxisListener(wl_listener* listener);
	void addFrameListener(wl_listener* listener);

	bool removeMotionListener(HandlerID hid);

	friend class EventHelper;
private:
	void motionCallback(void* data);
	wlr_cursor* cursor;
	wlr_xcursor_manager* xcursor_manager;
	std::list<std::pair<HandlerID, MotionCallback>> motionCallbacks;
	wl_listener motion;
};