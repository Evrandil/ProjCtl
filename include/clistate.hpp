#pragma once

#include <cstdint>

enum class CliState : std::uint8_t {
	NotInstalled,
	NotAuthenticated,
	Ready,
};
