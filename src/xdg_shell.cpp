#include "xdg_shell.h"

XDGShell::XDGShell(Display* display) {
	xdg_shell = wlr_xdg_shell_create(display->get());
	new_xdg_surface.notify = EventHelper::memberFunction<XDGShell, wl_listener, &XDGShell::new_xdg_surface, &XDGShell::onNewSurface>;
	wl_signal_add(&xdg_shell->events.new_surface,
		&new_xdg_surface);
}

void XDGShell::onNewSurface(void* data) {
	wlr_xdg_surface* xdg_surface = (wlr_xdg_surface*)data;

	if (xdg_surface->role != WLR_XDG_SURFACE_ROLE_TOPLEVEL) {
		return;
	}

	views.emplace_back(this, xdg_surface);
}

void XDGShell::renderViews(timespec when, wlr_output* output, wlr_output_layout* layout, wlr_renderer* renderer) {
	for (auto& view : views) {
		if (view.is_mapped()) {
			view.render(when, output, layout, renderer);
		}
	}
}

void XDGShell::removeView(View* view) {
	views.remove(*view);
}