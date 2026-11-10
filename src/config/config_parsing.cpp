#include "config_parsing.hpp"

#include "config.hpp"
#include "row.hpp"
#include "style.hpp"
#include "text.hpp"

#include <charconv>
#include <cstddef>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>
#include <vector>

namespace ConfigParsing {

	// ---- value parsing --------------------------------------------------------------------------
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

	// "number, title, state" -> {Number, Title, State}; unknown names are skipped
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

	// ---- one setter per section -----------------------------------------------------------------
	auto set_general(Settings::General& general, std::string_view key, std::string_view value) -> void {
		if (const std::optional<std::string Settings::General::*> field = lookup(GENERAL, key)) {
			general.*(*field) = value;
		}
	}

	auto set_width(Settings::Widths& widths, std::string_view key, std::string_view value) -> void {
		const std::optional<std::size_t Settings::Widths::*> field = lookup(WIDTHS, key);
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

	// <column>_width = 40   or   <column>_color = bold_red
	auto set_column_property(Settings::Column& column, std::string_view property, std::string_view value) -> void {
		if (property == "width") {
			if (const std::optional<std::size_t> width = parse_size(value)) {
				column.width = *width;
			}
		} else if (property == "color") {
			if (const std::optional<Style::Color> color = lookup(COLOR_NAMES, value)) {
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
		const std::optional<Style::Color Settings::Colors::*> field = lookup(COLORS, key);
		const std::optional<Style::Color>                     color = lookup(COLOR_NAMES, value);
		if (field && color) {
			colors.*(*field) = *color;
		}
	}

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
