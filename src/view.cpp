#include "view.h"
#include "util.h"

View::View(wlr_xdg_surface* xdg_surface)
	: xdg_surface(xdg_surface), uid(UID::GenUID()) {

		_surfaceMapped = CreateRef<EventHandler<void*>>(&xdg_surface->events.map);
		_surfaceMapped->addHandler([this](auto _) { mapped = true; });

		_surfaceUnmapped = CreateRef<EventHandler<void*>>(&xdg_surface->events.unmap);
		_surfaceUnmapped->addHandler([this](auto _) { mapped = false; });

		_surfaceDestroyed = CreateRef<EventHandler<void*>>(&xdg_surface->events.destroy);

		_surfaceRequestMove = CreateRef<EventHandler<void*>>(&xdg_surface->toplevel->events.request_move);
}

void View::render(timespec when, wlr_output* output, wlr_output_layout* layout, wlr_renderer* renderer) {

	renderSurface(xdg_surface->surface, when, output, layout, renderer, _x, _y);

}

// Recursively rendering surfaces from back to front
void View::renderSurface(wlr_surface* surface, timespec when, wlr_output* output,
	wlr_output_layout* layout, wlr_renderer* renderer, int x, int y) {

	// Rendering surfaces below

	wlr_subsurface* subsurface;
	wl_list_for_each(subsurface, &surface->current.subsurfaces_below, current.link) {
		wlr_subsurface_parent_state* state = &subsurface->current;
		int sx = state->x;
		int sy = state->y;

		renderSurface(subsurface->surface, when, output, layout, renderer, x + sx, y + sy);
	}

	// Rendering current surface
	renderSubsurface(surface, when, output, layout, renderer, x, y);
	// Rendering surfaces above
	wl_list_for_each(subsurface, &surface->current.subsurfaces_above, current.link) {
		wlr_subsurface_parent_state* state = &subsurface->current;
		int sx = state->x;
		int sy = state->y;

		renderSurface(subsurface->surface, when, output, layout, renderer, x + sx, y + sy);
	}

}

void View::renderSubsurface(wlr_surface* subsurface, timespec when, wlr_output* output,
	wlr_output_layout* output_layout, wlr_renderer* renderer, int x, int y) {

	wlr_texture* texture = wlr_surface_get_texture(subsurface);
	if (texture == nullptr)
		return;


	double ox = 0, oy = 0;
	wlr_output_layout_output_coords(
		output_layout, output, &ox, &oy
	);
	ox += x;
	oy += y;
	wlr_log(WLR_DEBUG, "View Coords (%lf, %lf)", ox, oy);

	wlr_box box = {
		.x = (int)(ox * output->scale),
		.y = (int)(oy * output->scale),
		.width = (int)(subsurface->current.width * output->scale),
		.height = (int)(subsurface->current.height * output->scale),
	};

	float matrix[9];
	wl_output_transform transform =
		wlr_output_transform_invert(subsurface->current.transform);
	wlr_matrix_project_box(matrix, &box, transform, 0, output->transform_matrix);

	wlr_render_texture_with_matrix(renderer, texture, matrix, 1);

	wlr_surface_send_frame_done(subsurface, &when);
}

bool View::hasSurfaceAt(double lx, double ly, wlr_surface** surface, double* sx, double* sy) {
	double view_sx = lx - _x;
	double view_sy = ly - _y;

	double _sx, _sy;
	wlr_surface* _surface = nullptr;
	_surface = wlr_xdg_surface_surface_at(xdg_surface, view_sx, view_sy, &_sx, &_sy);

	if (_surface != nullptr) {
		*sx = _sx;
		*sy = _sy;
		*surface = _surface;
		return true;
	}
	return false;
}

void View::setActivated() {
	wlr_xdg_toplevel_set_activated(xdg_surface, true);
}

void View::move(int x, int y) {
	_x = x;
	_y = y;
}

void View::move(double x, double y) {
	_x = x;
	_y = y;
}
