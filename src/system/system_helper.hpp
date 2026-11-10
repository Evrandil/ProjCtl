#pragma once

#include <cstddef>
#include <span>
#include <vector>
#include <string>
#include <sys/types.h>

constexpr std::size_t READ_CHUNK_SIZE  = 4096;
constexpr int         SIGNAL_EXIT_BASE = 128;

auto make_argv(std::span<const std::string> args) -> std::vector<char*>;
auto wait_for(pid_t pid) -> int;
