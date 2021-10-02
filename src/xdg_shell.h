#pragma once

#include "wl-roots-includes.h"
#include "view.h"
#include "event_helper.h"
#include "display.h"
#include "event_handler.h"
#include "event_signal.h"
#include "cursor.h"

#include <list>
#include <memory>
#include <optional>

class View;

class XDGShell {
public:
	XDGShell(Display* display);

	void removeView(View* view);
	void renderViews(timespec when, wlr_output* output, wlr_output_layout* layout, wlr_renderer* renderer);
	std::optional<std::reference_wrapper<View>> findViewAt(Cursor* cursor, wlr_surface** surface, double* sx, double* sy);
	std::optional<std::reference_wrapper<View>> findViewAt(double lx, double ly, wlr_surface** surface, double* sx, double* sy);
	void moveViewToTop(const View& view);

	Ref<EventSignal<View&>> surfaceRequestMove() { return _surfaceRequestMove; }

	friend class EventHandler<void*>;
private:
	void onNewSurface(wlr_xdg_surface* data);

	wlr_xdg_shell* xdg_shell;
	wl_listener new_xdg_surface;
	std::list<View> views;
	Ref<EventHandler<wlr_xdg_surface>> newSurfaceHandler;
	Ref<EventSignal<View&>> _surfaceRequestMove;
};