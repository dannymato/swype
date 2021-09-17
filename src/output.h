#pragma once
#include "wl-roots-includes.h"
#include "event_handler.h"
#include "util.h"
#include <functional>
#include <string_view>
#include <memory>

class EventHelper;

class Output {
public:
	Output(wlr_output* output);

	Ref<EventHandler<wlr_output>> destroyed() { return _destroyed; }
	Ref<EventHandler<wlr_output>> frame() { return _frame; }

	friend class Server;

private:
	void render(wlr_renderer* renderer, std::function<void()> renderAction);
	inline const std::pair<int, int> effectiveResolution();

	wlr_output* output;
	Ref<EventHandler<wlr_output>> _destroyed;
	Ref<EventHandler<wlr_output>> _frame;
};

struct OutputKey {

	OutputKey(wlr_output* output) : 
		OutputKey(output->make, output-> model, output->serial) {}

	OutputKey(char* make, char* model, char* serial)
		: make(make), model(model), serial(serial) {}

	bool operator==(const OutputKey& other) const {
		return make == other.make
			&& model == other.model
			&& serial == other.serial;
	}

	std::size_t hash() const {
		std::size_t h1 = std::hash<std::string_view>{}(make);
		std::size_t h2 = std::hash<std::string_view>{}(model);
		std::size_t h3 = std::hash<std::string_view>{}(serial);

		return h1 ^ h2 << 2 ^ h3 << 3;
	}

	std::string_view make;
	std::string_view model;
	std::string_view serial;

};

struct output_hash_fn {
	std::size_t operator() (const OutputKey& key) const {
		return key.hash();
	}
};