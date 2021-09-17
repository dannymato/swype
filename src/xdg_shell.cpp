#include "xdg_shell.h"
#include <functional>
#include <algorithm>
#include <iterator>

XDGShell::XDGShell(Display* display) {
	xdg_shell = wlr_xdg_shell_create(display->get());
	newSurfaceHandler = CreateRef<EventHandler<wlr_xdg_surface>>(&xdg_shell->events.new_surface);
	newSurfaceHandler->addHandler(std::bind(&XDGShell::onNewSurface, this, std::placeholders::_1));
}

void XDGShell::onNewSurface(wlr_xdg_surface* xdg_surface) {

	if (xdg_surface->role != WLR_XDG_SURFACE_ROLE_TOPLEVEL) {
		return;
	}
	wlr_log(WLR_DEBUG, "New surface surfaced");

	auto view = views.emplace_front(xdg_surface);
	view.surfaceDestroyed()->addHandler([view, this] (void* _) {
		views.remove(view);
	});
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

void XDGShell::moveViewToTop(const View& view) {
	auto it = std::find(views.begin(), views.end(), view);
	if (it != views.end()) {
		// Only rotates up to the needed view
		// Keeps the rest of the list in the same order
		// 1,2,3,4 -> 3,1,2,4
		std::rotate(views.begin(), it, std::next(it));
	} else {
		throw "Could not find view";
	}
}

std::optional<std::reference_wrapper<View>> XDGShell::findViewAt(Cursor* cursor, wlr_surface** surface, double* sx, double* sy) {
	return findViewAt(cursor->x(), cursor->y(), surface, sx, sy);
}

std::optional<std::reference_wrapper<View>> XDGShell::findViewAt(double lx, double ly, wlr_surface** surface, double* sx, double* sy) {
	auto it = std::find_if(begin(views), end(views),
		[lx, ly, surface, sx, sy] (auto view) {return view.hasSurfaceAt(lx, ly, surface, sx, sy);}
	);

	if (it == end(views)) {
		return std::nullopt;
	} else {
		return std::optional<std::reference_wrapper<View>> { *it };
	}
}