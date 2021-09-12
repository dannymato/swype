#pragma once

#include "wl-roots-includes.h"
#include "view.h"
#include "event_helper.h"
#include "display.h"

#include <list>
#include <memory>

class View;

class XDGShell {
public:
	XDGShell(Display* display);
	void removeView(View* view);
	void renderViews(timespec when, wlr_output* output, wlr_output_layout* layout, wlr_renderer* renderer);

	friend class EventHelper;
private:
	void onNewSurface(void* data);

	wlr_xdg_shell* xdg_shell;
	wl_listener new_xdg_surface;
	std::list<View> views;
};