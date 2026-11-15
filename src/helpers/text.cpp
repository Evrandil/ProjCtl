#include "text.hpp"

#include <cstddef>
#include <string_view>

constexpr std::size_t TRUNCATE_SIZE = 2;

auto trim(std::string_view text) -> std::string_view {
	const std::size_t first = text.find_first_not_of(" \t\r\n");

	if (first == std::string_view::npos) {
		return {};
	}

	const std::size_t last = text.find_last_not_of(" \t\r\n");

	return text.substr(first, last - first + 1);
}

auto truncate(std::string_view text, std::size_t max_length) -> std::string_view {
	if (text.size() <= max_length) {
		return text;
	}

	if (max_length <= TRUNCATE_SIZE) {
		return text.substr(0, max_length);
	}

	return text.substr(0, max_length - TRUNCATE_SIZE);
}
