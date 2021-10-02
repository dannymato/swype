#pragma once
#include "util.h"
#include <functional>
#include <forward_list>

template<typename T>
using SignalFunc = std::function<void(T)>;

template<typename T>
class EventSignal {
public:
	EventSignal() {
		_handlers = CreateRef<std::forward_list<SignalFunc<T>>>();
	}

	void AddHandler(SignalFunc<T> func) {
		_handlers->push_front(func);
	}

	void signalEvent(T data) {
		auto handlers = _handlers;
		for (auto& handler : *handlers) {
			handler(data);
		}
	}

private:

	Ref<std::forward_list<SignalFunc<T>>> _handlers;
};