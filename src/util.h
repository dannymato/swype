#pragma once

#include <random>
#include <limits>

static std::random_device rd;
static std::mt19937_64 gen(rd());
static std::uniform_int_distribution<uint64_t> dis(std::numeric_limits<uint64_t>::min(), std::numeric_limits<uint64_t>::max());

class UID {
public:
	static uint64_t GenUID() {
		return dis(gen);
	}
};