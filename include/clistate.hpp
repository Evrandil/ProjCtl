#pragma once

#include <cstdint>

enum class CliState : uint8_t
{
	NotInstalled,
	NotAuthenticated,
	Ready,
};
