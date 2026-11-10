#pragma once

#include "config.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string_view>
#include <utility>

namespace ConfigParsing {

	template <typename T>
	using Named = std::pair<std::string_view, T>;

	enum class Section : std::uint8_t {
		General,
		Widths,
		Tables,
		Colors,
		Unknown,
	};

	inline constexpr auto SECTIONS = std::to_array<Named<Section>>({
		{"General", Section::General},
		{"Widths", Section::Widths},
		{"Tables", Section::Tables},
		{"Colors", Section::Colors},
	});

	template <typename T, std::size_t N>
	constexpr auto lookup(const std::array<Named<T>, N>& table, std::string_view name) -> std::optional<T> {
		const auto found = std::ranges::find(table, name, &Named<T>::first);
		if (found == table.end()) {
			return std::nullopt;
		}
		return found->second;
	}

	auto apply_setting(Config& config, Section section, std::string_view key, std::string_view value) -> void;
} // namespace ConfigParsing
