#include "xdg_shell.h"
#include <functional>
#include <algorithm>
#include <iterator>
#include <ranges>

XDGShell::XDGShell(Display* display) {
	xdg_shell = wlr_xdg_shell_create(display->get());
	_newSurfaceHandler = CreateRef<EventHandler<wlr_xdg_surface>>(&xdg_shell->events.new_surface);
	_newSurfaceHandler->addHandler(std::bind(&XDGShell::onNewSurface, this, std::placeholders::_1));
	_surfaceRequestMove = CreateRef<EventSignal<Ref<View>>>();
}

void XDGShell::onNewSurface(wlr_xdg_surface* xdg_surface) {

	if (xdg_surface->role != WLR_XDG_SURFACE_ROLE_TOPLEVEL) {
		return;
	}
	wlr_log(WLR_DEBUG, "New surface surfaced");
	auto view = CreateRef<View>(xdg_surface);

	views.push_front(view);
	view->surfaceDestroyed()->addHandler([view, this] (void* _) {
		views.remove(view);
	});

	view->surfaceRequestMove()->addHandler([view, this] (void* _) mutable {
		_surfaceRequestMove->signalEvent(view);
	});

	clearActivated();
	view->activate();
}

void XDGShell::renderViews(timespec when, wlr_output* output, wlr_output_layout* layout, wlr_renderer* renderer) {
	for (auto& view : std::ranges::reverse_view(views)) {
		if (view->is_mapped()) {
			view->render(when, output, layout, renderer);
		}
	}
}

void XDGShell::removeView(Ref<View> view) {
	views.remove(view);
}

void XDGShell::moveViewToTop(Ref<View> view) {
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

std::optional<Ref<View>> XDGShell::findViewAt(Ref<Cursor> cursor, wlr_surface** surface, double* sx, double* sy) {
	return findViewAt(cursor->x(), cursor->y(), surface, sx, sy);
}

std::optional<Ref<View>> XDGShell::findViewAt(double lx, double ly, wlr_surface** surface, double* sx, double* sy) {
	auto it = std::find_if(begin(views), end(views),
		[lx, ly, surface, sx, sy] (auto view) {return view->hasSurfaceAt(lx, ly, surface, sx, sy);}
	);

	if (it == end(views)) {
		return std::nullopt;
	} else {
		return std::optional<Ref<View>> { *it };
	}
}

std::optional<Ref<View>> XDGShell::topView() {
	if (begin(views) != end(views)) {
		return std::optional<Ref<View>> {*begin(views)};
	}
	return std::nullopt;
}

void XDGShell::clearActivated() {
	for (auto view : views) {
		if (view->isActivated()) {
			view->deactivate();
		}
	}
}
