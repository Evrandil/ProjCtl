#pragma once

#include <string_view>
#include <optional>
#include <string>
#include <cstdint>

namespace ProjectsParsing {
	enum class Line : std::uint8_t {
		Section,
		Path,
		Run,
		Build,
		Unknown,
	};

	auto key_of(std::string_view entry) -> std::string_view;
	auto value_of(std::string_view entry) -> std::string_view;
	auto non_empty(std::string_view value) -> std::optional<std::string>;
	auto line_of(std::string_view line) -> Line;
}
