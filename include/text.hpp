#pragma once

#include <cstddef>
#include <string>
#include <string_view>

auto trim(std::string_view text) -> std::string_view;
auto truncate(std::string_view text, std::size_t max_length) -> std::string;
