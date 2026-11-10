#pragma once

#include "row.hpp"
#include "config.hpp"
#include "style.hpp"

#include <string_view>
#include <string>
#include <utility>
#include <array>
#include <cstdint>
#include <cstddef>
#include <optional>
#include <algorithm>
#include <vector>


namespace ConfigParsing {
	// ---- table types ----------------------------------------------------------------------------

	template <typename T>
	using Named = std::pair<std::string_view, T>;

	template <typename Owner, typename T>
	using Field = Named<T Owner::*>;

	struct ColumnInfo {
		std::string_view name;
		TableField       field;
		Settings::Column Settings::Tables::* member;
	};

	enum class Section : std::uint8_t {
		General,
		Widths,
		Tables,
		Colors,
		Unknown,
	};

	// ---- tables ---------------------------------------------------------------------------------

	inline constexpr auto SECTIONS = std::to_array<Named<Section>>({
		{"General", Section::General},
		{"Widths", Section::Widths},
		{"Tables", Section::Tables},
		{"Colors", Section::Colors},
	});

	inline constexpr auto GENERAL = std::to_array<Field<Settings::General, std::string>>({
		{"editor", &Settings::General::editor},
	});

	inline constexpr auto WIDTHS = std::to_array<Field<Settings::Widths, std::size_t>>({
		{"max_width", &Settings::Widths::max},
		{"name_width", &Settings::Widths::name},
		{"path_width", &Settings::Widths::path},
		{"remote_width", &Settings::Widths::remote},
		{"branch_width", &Settings::Widths::branch},
	});

	inline constexpr auto COLUMNS = std::to_array<ColumnInfo>({
		{.name = "number", .field = TableField::Number, .member = &Settings::Tables::number},
		{.name = "title", .field = TableField::Title, .member = &Settings::Tables::title},
		{.name = "state", .field = TableField::State, .member = &Settings::Tables::state},
		{.name = "author", .field = TableField::Author, .member = &Settings::Tables::author},
		{.name = "labels", .field = TableField::Labels, .member = &Settings::Tables::labels},
		{.name = "created", .field = TableField::Created, .member = &Settings::Tables::created},
		{.name = "updated", .field = TableField::Updated, .member = &Settings::Tables::updated},
		{.name = "parent", .field = TableField::Parent, .member = &Settings::Tables::parent},
	});

	inline constexpr auto COLORS = std::to_array<Field<Settings::Colors, Style::Color>>({
		{"header", &Settings::Colors::header},
		{"project_name", &Settings::Colors::name},
		{"project_path", &Settings::Colors::path},
		{"project_branch", &Settings::Colors::branch},
		{"project_remote", &Settings::Colors::remote},
		{"project_type", &Settings::Colors::type},
		{"project_short_remote", &Settings::Colors::short_remote},
		{"open", &Settings::Colors::open},
		{"closed", &Settings::Colors::closed},
		{"tick", &Settings::Colors::tick},
		{"cross", &Settings::Colors::cross},
		{"error", &Settings::Colors::error},
		{"warning", &Settings::Colors::warning},
	});

	inline constexpr auto COLOR_NAMES = std::to_array<Named<Style::Color>>({
		{"default", Style::Color::Default},
		{"black", Style::Color::Black},
		{"bold_black", Style::Color::BoldBlack},
		{"red", Style::Color::Red},
		{"bold_red", Style::Color::BoldRed},
		{"green", Style::Color::Green},
		{"bold_green", Style::Color::BoldGreen},
		{"yellow", Style::Color::Yellow},
		{"bold_yellow", Style::Color::BoldYellow},
		{"blue", Style::Color::Blue},
		{"bold_blue", Style::Color::BoldBlue},
		{"magenta", Style::Color::Magenta},
		{"bold_magenta", Style::Color::BoldMagenta},
		{"cyan", Style::Color::Cyan},
		{"bold_cyan", Style::Color::BoldCyan},
		{"white", Style::Color::White},
		{"bold_white", Style::Color::BoldWhite},
	});

	// ---- lookups --------------------------------------------------------------------------------

	template <typename T, std::size_t N>
	constexpr auto lookup(const std::array<Named<T>, N>& table, std::string_view name) -> std::optional<T> {
		const auto found = std::ranges::find(table, name, &Named<T>::first);
		if (found == table.end()) {
			return std::nullopt;
		}
		return found->second;
	}

	constexpr auto find_column(std::string_view name) -> std::optional<ColumnInfo> {
		const auto* const found = std::ranges::find(COLUMNS, name, &ColumnInfo::name);
		if (found == COLUMNS.end()) {
			return std::nullopt;
		}
		return *found;
	}

	static_assert(lookup(SECTIONS, "Widths") == Section::Widths);
	static_assert(lookup(COLOR_NAMES, "bold_red") == Style::Color::BoldRed);
	static_assert(find_column("parent")->field == TableField::Parent);
	static_assert(!find_column("nope").has_value());

	// ---- value parsing --------------------------------------------------------------------------
	auto parse_size(std::string_view text) -> std::optional<std::size_t>;
	// "number, title, state" -> {Number, Title, State}; unknown names are skipped
	auto parse_column_list(std::string_view value) -> std::vector<TableField>;

	// ---- one setter per section -----------------------------------------------------------------
	auto set_general(Settings::General& general, std::string_view key, std::string_view value) -> void;
	auto set_width(Settings::Widths& widths, std::string_view key, std::string_view value) -> void;
	auto set_column_list(std::vector<TableField>& target, std::string_view value) -> void;
	// <column>_width = 40   or   <column>_color = bold_red
	auto set_column_property(Settings::Column& column, std::string_view property, std::string_view value) -> void;
	auto set_table(Settings::Tables& tables, std::string_view key, std::string_view value) -> void;
	auto set_color(Settings::Colors& colors, std::string_view key, std::string_view value) -> void;
	auto apply_setting(Config& config, Section section, std::string_view key, std::string_view value) -> void;
}
