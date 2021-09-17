#include <iostream>
#include <ctime>
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

	renderAction();

	wlr_output_render_software_cursors(output, nullptr);
	wlr_renderer_end(renderer);
	wlr_output_commit(output);
}

inline const std::pair<int, int> Output::effectiveResolution() {
	int width, height;
	wlr_output_effective_resolution(output, &width, &height);
	return std::make_pair(width, height);
}
