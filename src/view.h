#pragma once

#include "wl-roots-includes.h"
#include "event_helper.h"
#include "xdg_shell.h"

class XDGShell;

class View {
public:
	View(XDGShell* shell, wlr_xdg_surface* xdg_surface);

	bool operator==(const View& other) {
		return other.uid == uid;
	}

	inline bool is_mapped() { return mapped; }

	void render(timespec when, wlr_output* output, wlr_output_layout* layout, wlr_renderer* renderer);

	friend class EventHelper;
private:
	void onSurfaceMap(void* data);
	void onSurfaceUnmap(void* data);
	// Figure out how to actually do this
	void onSurfaceDestroy(void* data);

	static void renderSurface(wlr_surface* surface, timespec when, wlr_output* output, wlr_output_layout* layout,
	 wlr_renderer* renderer, int x, int y);

	XDGShell* shell; // Don't love this but gotta do it to setup destroying
	wlr_xdg_surface* xdg_surface;
	wl_listener surface_mapped;
	wl_listener surface_unmapped;
	wl_listener surface_destroyed;
	bool mapped;
	int x, y;
	uint64_t uid;
};