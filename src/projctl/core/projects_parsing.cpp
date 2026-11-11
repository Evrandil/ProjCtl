#include "projects_parsing.hpp"

#include "text.hpp"

#include <string_view>
#include <optional>
#include <cstddef>
#include <string>


auto ProjectsParsing::key_of(const std::string_view entry) -> std::string_view {
	if (const std::size_t separator = entry.find('='); separator != std::string_view::npos) {
		return trim(entry.substr(0, separator));
	}
	return {};
}

auto ProjectsParsing::value_of(const std::string_view entry) -> std::string_view {
	if (const std::size_t separator = entry.find('='); separator != std::string_view::npos) {
		return trim(entry.substr(separator + 1));
	}
	return {};
}

auto ProjectsParsing::non_empty(const std::string_view value) -> std::optional<std::string> {
	if (value.empty()) {
		return std::nullopt;
	}
	return std::string{value};
}

auto ProjectsParsing::line_of(const std::string_view line) -> Line {
	if (line.front() == '[' && line.back() == ']') {
		return Line::Section;
	}
	const std::string_view header = key_of(line);
	if (header == "path") {
		return Line::Path;
	}
	if (header == "run") {
		return Line::Run;
	}
	if (header == "build") {
		return Line::Build;
	}
	return Line::Unknown;
}
