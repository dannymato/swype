#include "view.h"
#include "util.h"

View::View(XDGShell* shell, wlr_xdg_surface* xdg_surface) 
	:  shell(shell), xdg_surface(xdg_surface), uid(UID::GenUID()) {

	surface_mapped.notify =
		&EventHelper::memberFunction<View, wl_listener, &View::surface_mapped, &View::onSurfaceMap>;
	wl_signal_add(&xdg_surface->events.map, &surface_mapped);

	surface_unmapped.notify =
		&EventHelper::memberFunction<View, wl_listener, &View::surface_unmapped, &View::onSurfaceUnmap>;
	wl_signal_add(&xdg_surface->events.unmap, &surface_unmapped);

	surface_destroyed.notify =
		&EventHelper::memberFunction<View, wl_listener, &View::surface_destroyed, &View::onSurfaceDestroy>;
	wl_signal_add(&xdg_surface->events.destroy, &surface_destroyed);
}

void View::onSurfaceMap(void* _) {
	wlr_log(WLR_DEBUG, "Surface Mapped UID: %li", uid);
	mapped = true;
}

void View::onSurfaceUnmap(void* _) {
	wlr_log(WLR_DEBUG, "Surface Unmapped UID: %li", uid);
	mapped = false;
}

void View::onSurfaceDestroy(void* _) {
	wlr_log(WLR_DEBUG, "Surface Destroyed UID: %li", uid);
	shell->removeView(this);
}

// TODO: Find out about sx and sy stuff will probably render in the wrong place now
void View::render(timespec when, wlr_output* output, wlr_output_layout* layout, wlr_renderer* renderer) {
	
	renderSurface(xdg_surface->surface, when, output, layout, renderer, 0, 0);

}

void View::renderSurface(wlr_surface* surface, timespec when, wlr_output* output, 
	wlr_output_layout* layout, wlr_renderer* renderer, int x, int y) {

	wlr_subsurface* subsurface;
	wl_list_for_each(subsurface, &surface->current.subsurfaces_below, current.link) {
		wlr_subsurface_parent_state* state = &subsurface->current;
		int sx = state->x;
		int sy = state->y;

		renderSurface(subsurface->surface, when, output, layout, renderer, x + sx, y + sy);
	}

	wlr_texture* texture = wlr_surface_get_texture(surface);
	if (texture == nullptr) return;

	double ox = 0, oy = 0;
	wlr_output_layout_output_coords(
		layout, output, &ox, &oy
	);
	ox += x;
	oy += y;

	wlr_box box = {
		.x = (int)(ox * output->scale),
		.y = (int)(oy * output->scale),
		.width = (int)(surface->current.width * output->scale),
		.height = (int)(surface->current.height * output->scale),
	};

	float matrix[9];
	wl_output_transform transform =
		wlr_output_transform_invert(surface->current.transform);
	wlr_matrix_project_box(matrix, &box, transform, 0, output->transform_matrix);

	wlr_render_texture_with_matrix(renderer, texture, matrix, 1);

	wlr_surface_send_frame_done(surface, &when);

	wl_list_for_each(subsurface, &surface->current.subsurfaces_above, current.link) {
		wlr_subsurface_parent_state* state = &subsurface->current;
		int sx = state->x;
		int sy = state->y;

		renderSurface(subsurface->surface, when, output, layout, renderer, x + sx, y + sy);
	}

}

