#include "text.hpp"

#include <cstddef>
#include <string>
#include <string_view>

constexpr std::size_t TRUNCATE_SIZE = 3;

auto trim(std::string_view text) -> std::string_view {
	const std::size_t first = text.find_first_not_of(" \t\r\n");

	if (first == std::string_view::npos) {
		return {};
	}

	const std::size_t last = text.find_last_not_of(" \t\r\n");

	return text.substr(first, last - first + 1);
}

auto truncate(std::string_view text, std::size_t max_length) -> std::string {
	if (text.size() <= max_length) {
		return std::string(text);
	}

	if (max_length <= TRUNCATE_SIZE) {
		return std::string(text.substr(0, max_length));
	}

	return std::string(text.substr(0, max_length - TRUNCATE_SIZE)) + "...";
}
