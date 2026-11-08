#pragma once

#include "command.hpp"
#include <span>
#include <string_view>

auto parser(std::span<const std::string_view>) -> Command;
