#pragma once

#include "wl-roots-includes.h"
#include "cursor.h"
#include "display.h"
#include "event_handler.h"
#include "event_helper.h"
#include "event_signal.h"
#include "view.h"

#include <list>
#include <memory>
#include <optional>

class XDGShell {
public:
	XDGShell(Display* display);
	~XDGShell();

	void removeView(Ref<View> view);
	void renderViews(timespec when, wlr_output* output, wlr_output_layout* layout, wlr_renderer* renderer);
	std::optional<Ref<View>> findViewAt(Ref<Cursor> cursor, wlr_surface** surface, double* sx, double* sy);
	std::optional<Ref<View>> findViewAt(double lx, double ly, wlr_surface** surface, double* sx, double* sy);
	std::optional<Ref<View>> topView();
	void moveViewToTop(Ref<View> view);
	void clearActivated();

	Ref<EventSignal<Ref<View>>> surfaceRequestMove() { return _surfaceRequestMove; }
	Ref<EventHandler<wlr_xdg_surface>> newSurfaceHandler() { return _newSurfaceHandler; }

private:
	void onNewSurface(wlr_xdg_surface* data);

	wlr_xdg_shell* xdg_shell;
	wl_listener new_xdg_surface;
	std::list<Ref<View>> views;
	Ref<EventHandler<wlr_xdg_surface>> _newSurfaceHandler;
	Ref<EventSignal<Ref<View>>> _surfaceRequestMove;
};