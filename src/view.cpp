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

void View::onSurfaceMap(void* data) {
	wlr_log(WLR_DEBUG, "Surface Mapped UID: %i", uid);
	mapped = true;
}

void View::onSurfaceUnmap(void* data) {
	wlr_log(WLR_DEBUG, "Surface Unmapped UID: %i", uid);
	mapped = false;
}

void View::onSurfaceDestroy(void* data) {
	wlr_log(WLR_DEBUG, "Surface Destroyed UID: %i", uid);
	shell->removeView(this);
}

// TODO: Find out about sx and sy stuff will probably render in the wrong place now
void View::render(timespec when, wlr_output* output, wlr_output_layout* layout, wlr_renderer* renderer) {
	wlr_texture* texture = wlr_surface_get_texture(xdg_surface->surface);
	if (texture == nullptr) return;

	double ox = 0, oy = 0;
	wlr_output_layout_output_coords(
		layout, output, &ox, &oy
	);
	ox += x;
	oy += y;

	wlr_box box = {
		.x = ox * output->scale,
		.y = oy * output->scale,
		.width = xdg_surface->surface->current.width * output->scale,
		.height = xdg_surface->surface->current.width * output->scale,
	};

	float matrix[9];
	wl_output_transform transform =
		wlr_output_transform_invert(xdg_surface->surface->current.transform);
	wlr_matrix_project_box(matrix, &box, transform, 0, output->transform_matrix);

	wlr_render_texture_with_matrix(renderer, texture, matrix, 1);

	wlr_surface_send_frame_done(xdg_surface->surface, &when);

}

void View::renderSurface(wlr_surface* surface, timespec when, wlr_output* output, 
	wlr_output_layout* layout, wlr_renderer* renderer, int x, int y) {

	wlr_subsurface* subsurface;
	wl_list_for_each(subsurface, &surface->current.subsurfaces_below, current.link) {
		
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
		.x = ox * output->scale,
		.y = oy * output->scale,
		.width = surface->current.width * output->scale,
		.height = surface->current.width * output->scale,
	};

	float matrix[9];
	wl_output_transform transform =
		wlr_output_transform_invert(surface->current.transform);
	wlr_matrix_project_box(matrix, &box, transform, 0, output->transform_matrix);

	wlr_render_texture_with_matrix(renderer, texture, matrix, 1);

	wlr_surface_send_frame_done(surface, &when);

}

