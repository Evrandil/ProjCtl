#pragma once

#include <span>
#include <string_view>

#include "command.hpp"

auto parser(std::span<const std::string_view> input) -> Command;
