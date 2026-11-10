#pragma once

#include <string>
#include <string_view>
#include <cstddef>


auto trim(std::string_view text) -> std::string_view;
auto truncate(std::string_view text, std::size_t max_length) -> std::string;

