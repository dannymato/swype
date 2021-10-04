#pragma once
#include <string>
#include <unistd.h>
#include "wl-roots-includes.h"

template<typename T, class... Ts>
concept all_convertible =
	std::conjunction_v<std::is_convertible<Ts, T>...>;

class Command {
public:
	template <typename ...Args> requires all_convertible<const char*, Args...>
	static void Exec(std::string command, Args&&... args) {
		Exec(command.c_str(), args...);
	}

	template <typename ...Args> requires all_convertible<const char*, Args...>
	static void Exec(const char* command, Args&&... args) {
		pid_t child;
		if ((child = fork()) == 0) {
			execlp(command, command, std::forward<Args>(args)..., (const char*)nullptr);
		}
		wlr_log(WLR_DEBUG, "Forked process %s with PID: %i", command, child);
	}
};