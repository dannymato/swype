#include "output.h"
#include "event_helper.h"

Output::Output(wlr_output* output)
: output(output) {
	_destroyed = CreateRef<EventHandler<wlr_output>>(&output->events.destroy);
	_frame = CreateRef<EventHandler<wlr_output>>(&output->events.frame);
}

void Output::render(wlr_renderer* renderer, std::function<void()> renderAction) {
	if (!wlr_output_attach_render(output, nullptr))
		return;
	auto &[width, height] = effectiveResolution();
	wlr_renderer_begin(renderer, width, height);

	// Set the clear color
	const float color[4] = {0.3, 0.3, 0.3, 1.0};
	wlr_renderer_clear(renderer, color);

	// Calling function can do whatever they want to the view here
	renderAction();

	// Won't do anything if hardware cursors are available
	wlr_output_render_software_cursors(output, nullptr);
	wlr_renderer_end(renderer);
	wlr_output_commit(output);
}

inline const std::pair<int, int> Output::effectiveResolution() {
	int width, height;
	wlr_output_effective_resolution(output, &width, &height);
	return std::make_pair(width, height);
}
