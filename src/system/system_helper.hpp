#pragma once

#include <cstddef>
#include <span>
#include <string>
#include <sys/types.h>
#include <vector>

namespace SystemHelper {
	inline constexpr std::size_t READ_CHUNK_SIZE  = 4096;
	inline constexpr int         SIGNAL_EXIT_BASE = 128; // shell convention: killed by signal N -> exit code 128 + N

	auto make_argv(std::span<const std::string> args) -> std::vector<char*>;
	auto wait_for(pid_t pid) -> int;
} // namespace SystemHelper
