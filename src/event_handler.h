#pragma once
#include "wl-roots-includes.h"
#include "event_helper.h"
#include "util.h"

#include <functional>
#include <map>

template<typename T>
using HandlerFunc = std::function<void(T*)>;

typedef uint64_t HandlerID;

template<typename T>
using CallbackMap = std::unordered_map<HandlerID, HandlerFunc<T>>;

// This class will act as a wrapper for other signals
// It will listen for events on the wl_signal and dispatch them to stored std::functions
template<typename T>
class EventHandler {
public:

	EventHandler(wl_signal* signal) {
		_callbacks = CreateRef<CallbackMap<T>>();
		listener.notify = 
			&EventHelper::memberFunction<EventHandler<T>, wl_listener, &EventHandler::listener, &EventHandler::handleEvent>; 
		wl_signal_add(signal, &listener);
	}

	~EventHandler() {
		// Stop subscribing to events
		wl_list_remove(&listener.link);
	}

	HandlerID addHandler(HandlerFunc<T> func) {
		HandlerID id = UID::GenUID();
		(*_callbacks)[id] = func;
		return id;
	}

	bool removeHandler(HandlerID id) {
		return _callbacks->erase(id) > 0;
	}

	friend class EventHelper;
private:
	void handleEvent(void* data) {
		T* castedData = (T*)data;
		// Hanging onto a reference to the callbacks so they don't get deleted from under us
		auto callbacks = _callbacks;
		for (const auto& [id, func] : *callbacks) {
			func(castedData);
		}
	}

	wl_listener listener;
	Ref<CallbackMap<T>> _callbacks;
};