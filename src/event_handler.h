#pragma once
#include "wl-roots-includes.h"
#include "event_helper.h"
#include "util.h"

#include <functional>
#include <map>

template<typename T>
using HandlerFunc = std::function<void(T*)>;

typedef uint64_t HandlerID;

// This class will act as a wrapper for other signals
// It will listen for events on the wl_signal and dispatch them to stored std::functions
template<typename T>
class EventHandler {
public:
	EventHandler() = default;
	EventHandler(wl_signal* signal) {
		listener.notify = 
			&EventHelper::memberFunction<EventHandler<T>, wl_listener, &EventHandler::listener, &EventHandler::handleEvent>; 
		wl_signal_add(signal, &listener);
	}

	HandlerID addHandler(HandlerFunc<T> func) {
		HandlerID id = UID::GenUID();
		callbacks[id] = func;
		return id;
	}

	bool removeHandler(HandlerID id) {
		return callbacks.erase(id) > 0;
	}

	friend class EventHelper;
private:
	void handleEvent(void* data) {
		T* castedData = (T*)data;
		for (const auto& [id, func] : callbacks) {
			func(castedData);
		}
	}

	wl_listener listener;
	std::unordered_map<HandlerID, HandlerFunc<T>> callbacks;
};