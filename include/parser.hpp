#pragma once

#include "command.hpp"
#include <span>
#include <string_view>


Command parser(std::span<const std::string_view>);
