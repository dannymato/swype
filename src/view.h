#pragma once

#include "wl-roots-includes.h"
#include "event_handler.h"

class View {
public:
	View(wlr_xdg_surface* xdg_surface);

	bool operator==(const View& other) {
		return other.uid == uid;
	}

	inline bool isMapped() { return mapped; }
	int x() const { return _x; }
	int y() const { return _y; }
	inline bool isFocused() { return activated;	}

	wlr_surface* getSurface() { return xdg_surface->surface; }
	wlr_xdg_surface* getXDGSurface() { return xdg_surface; }

	void render(timespec when, wlr_output* output, wlr_output_layout* layout, wlr_renderer* renderer);
	bool hasSurfaceAt(double lx, double ly, wlr_surface** surface, double* sx, double* sy);
	void focus();
	void defocus();
	void move(int x, int y);
	void move (double x, double y);
	void requestClose();

	Ref<EventHandler<void*>> surfaceDestroyed() { return _surfaceDestroyed; }
	Ref<EventHandler<void*>> surfaceRequestMove() { return _surfaceRequestMove; }

private:
	void renderSurface(wlr_surface* surface, timespec when, wlr_output* output, wlr_output_layout* layout,
	 wlr_renderer* renderer, int x, int y);

	void renderSubsurface(wlr_surface* subsurface, timespec when, wlr_output* output,
	wlr_output_layout* output_layout, wlr_renderer* renderer, int x, int y);

	Ref<EventHandler<void*>> _surfaceMapped;
	Ref<EventHandler<void*>> _surfaceUnmapped;
	Ref<EventHandler<void*>> _surfaceDestroyed;
	Ref<EventHandler<void*>> _surfaceRequestMove;

	wlr_xdg_surface* xdg_surface;
	bool mapped;
	int _x = 0;
	int _y = 0;
	uint64_t uid;
	bool activated = false;
};