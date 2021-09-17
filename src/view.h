#pragma once

#include "wl-roots-includes.h"
#include "event_handler.h"

class View {
public:
	View(wlr_xdg_surface* xdg_surface);

	bool operator==(const View& other) {
		return other.uid == uid;
	}

	inline bool is_mapped() { return mapped; }

	wlr_surface* getSurface() { return xdg_surface->surface; }
	wlr_xdg_surface* getXDGSurface() { return xdg_surface; }

	void render(timespec when, wlr_output* output, wlr_output_layout* layout, wlr_renderer* renderer);
	bool hasSurfaceAt(double lx, double ly, wlr_surface** surface, double* sx, double* sy);
	void setActivated();

	Ref<EventHandler<void*>> surfaceDestroyed() { return _surfaceDestroyed; }

private:
	static void renderSurface(wlr_surface* surface, timespec when, wlr_output* output, wlr_output_layout* layout,
	 wlr_renderer* renderer, int x, int y);

	Ref<EventHandler<void*>> _surfaceMapped;
	Ref<EventHandler<void*>> _surfaceUnmapped;
	Ref<EventHandler<void*>> _surfaceDestroyed;

	wlr_xdg_surface* xdg_surface;
	bool mapped;
	int x, y;
	uint64_t uid;
};