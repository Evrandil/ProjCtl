#include "config_parsing.hpp"

#include "config.hpp"
#include "row.hpp"
#include "style.hpp"
#include "text.hpp"

#include <algorithm>
#include <array>
#include <charconv>
#include <cstddef>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>
#include <vector>

namespace {
	struct ColumnInfo {
		std::string_view name;
		TableField       field;
		Settings::Column Settings::Tables::* member;
	};

	template <typename Owner, typename T>
	using Field = ConfigParsing::Named<T Owner::*>;

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

	inline constexpr auto COLOR_NAMES = std::to_array<ConfigParsing::Named<Style::Color>>({
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

	constexpr auto find_column(std::string_view name) -> std::optional<ColumnInfo> {
		const auto* const found = std::ranges::find(COLUMNS, name, &ColumnInfo::name);
		if (found == COLUMNS.end()) {
			return std::nullopt;
		}
		return *found;
	}

	auto parse_size(std::string_view text) -> std::optional<std::size_t> {
		std::size_t       value = 0;
		const char* const first = std::to_address(text.begin());
		const char* const last  = std::to_address(text.end());
		const auto [end, error] = std::from_chars(first, last, value);
		if (error != std::errc{} || end != last) {
			return std::nullopt;
		}
		return value;
	}

	auto parse_column_list(std::string_view value) -> std::vector<TableField> {
		std::vector<TableField> fields;
		while (!value.empty()) {
			const std::size_t               space  = value.find(' ');
			const std::optional<ColumnInfo> column = find_column(trim(value.substr(0, space)));
			if (column) {
				fields.push_back(column->field);
			}
			value = space == std::string_view::npos ? std::string_view{} : value.substr(space + 1);
		}
		return fields;
	}

	auto set_general(Settings::General& general, std::string_view key, std::string_view value) -> void {
		if (const std::optional<std::string Settings::General::*> field = ConfigParsing::lookup(GENERAL, key)) {
			general.*(*field) = value;
		}
	}

	auto set_width(Settings::Widths& widths, std::string_view key, std::string_view value) -> void {
		const std::optional<std::size_t Settings::Widths::*> field = ConfigParsing::lookup(WIDTHS, key);
		const std::optional<std::size_t>                     width = parse_size(value);
		if (field && width) {
			widths.*(*field) = *width;
		}
	}

	auto set_column_list(std::vector<TableField>& target, std::string_view value) -> void {
		std::vector<TableField> parsed = parse_column_list(value);
		if (!parsed.empty()) {
			target = std::move(parsed);
		}
	}

	auto set_column_property(Settings::Column& column, std::string_view property, std::string_view value) -> void {
		if (property == "width") {
			if (const std::optional<std::size_t> width = parse_size(value)) {
				column.width = *width;
			}
		} else if (property == "color") {
			if (const std::optional<Style::Color> color = ConfigParsing::lookup(COLOR_NAMES, value)) {
				column.color = *color;
			}
		}
	}

	auto set_table(Settings::Tables& tables, std::string_view key, std::string_view value) -> void {
		if (key == "issues") {
			set_column_list(tables.issue_columns, value);
			return;
		}
		if (key == "requests") {
			set_column_list(tables.request_columns, value);
			return;
		}

		const std::size_t               underscore = key.rfind('_');
		const std::optional<ColumnInfo> column     = find_column(key.substr(0, underscore));
		// TODO this underscore chech isn't rly checking - if it's std::string::npos column will throw nullopt anyway
		if (underscore == std::string_view::npos || !column) {
			return;
		}
		set_column_property(tables.*column->member, key.substr(underscore + 1), value);
	}

	auto set_color(Settings::Colors& colors, std::string_view key, std::string_view value) -> void {
		if (key == "enabled") {
			colors.enabled = value == "true";
			return;
		}
		const std::optional<Style::Color Settings::Colors::*> field = ConfigParsing::lookup(COLORS, key);
		const std::optional<Style::Color>                     color = ConfigParsing::lookup(COLOR_NAMES, value);
		if (field && color) {
			colors.*(*field) = *color;
		}
	}
} // namespace

namespace ConfigParsing {
	auto apply_setting(Config& config, Section section, std::string_view key, std::string_view value) -> void {
		switch (section) {
			case Section::General: set_general(config.general, key, value); break;
			case Section::Widths:  set_width(config.widths, key, value); break;
			case Section::Tables:  set_table(config.tables, key, value); break;
			case Section::Colors:  set_color(config.colors, key, value); break;
			case Section::Unknown: break;
		}
	}
} // namespace ConfigParsing
