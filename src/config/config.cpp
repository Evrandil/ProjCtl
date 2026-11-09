#include "config.hpp"
#include "row.hpp"
#include "style.hpp"

#include <algorithm>
#include <array>
#include <charconv>
#include <cstddef>
#include <cstdint>
#include <expected>
#include <filesystem>
#include <fstream>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>
#include <vector>

namespace {
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

	constexpr auto SECTIONS = std::to_array<Named<Section>>({
		{"General", Section::General},
		{"Widths", Section::Widths},
		{"Tables", Section::Tables},
		{"Colors", Section::Colors},
	});

	constexpr auto GENERAL = std::to_array<Field<Settings::General, std::string>>({
		{"editor", &Settings::General::editor},
	});

	constexpr auto WIDTHS = std::to_array<Field<Settings::Widths, std::size_t>>({
		{"max_width", &Settings::Widths::max},
		{"name_width", &Settings::Widths::name},
		{"path_width", &Settings::Widths::path},
		{"remote_width", &Settings::Widths::remote},
		{"branch_width", &Settings::Widths::branch},
	});

	constexpr auto COLUMNS = std::to_array<ColumnInfo>({
		{.name = "number", .field = TableField::Number, .member = &Settings::Tables::number},
		{.name = "title", .field = TableField::Title, .member = &Settings::Tables::title},
		{.name = "state", .field = TableField::State, .member = &Settings::Tables::state},
		{.name = "author", .field = TableField::Author, .member = &Settings::Tables::author},
		{.name = "labels", .field = TableField::Labels, .member = &Settings::Tables::labels},
		{.name = "created", .field = TableField::Created, .member = &Settings::Tables::created},
		{.name = "updated", .field = TableField::Updated, .member = &Settings::Tables::updated},
		{.name = "parent", .field = TableField::Parent, .member = &Settings::Tables::parent},
	});

	constexpr auto COLORS = std::to_array<Field<Settings::Colors, Style::Color>>({
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

	constexpr auto COLOR_NAMES = std::to_array<Named<Style::Color>>({
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

	auto trim(std::string_view text) -> std::string_view {
		const std::size_t first = text.find_first_not_of(" \t\r\n");
		if (first == std::string_view::npos) {
			return {};
		}
		const std::size_t last = text.find_last_not_of(" \t\r\n");
		return text.substr(first, last - first + 1);
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
} // namespace

auto column_of(const Settings::Tables& tables, TableField field) -> const Settings::Column& {
	switch (field) {
		case TableField::Number:  return tables.number;
		case TableField::Title:   return tables.title;
		case TableField::State:   return tables.state;
		case TableField::Author:  return tables.author;
		case TableField::Labels:  return tables.labels;
		case TableField::Created: return tables.created;
		case TableField::Updated: return tables.updated;
		case TableField::Parent:  return tables.parent;
	}
	std::unreachable();
}

auto load_config(const std::filesystem::path& path) -> std::expected<Config, std::string> {
	std::ifstream config_file(path);
	if (!config_file) {
		return std::unexpected("Failed to open config file: " + path.string());
	}

	Config      config;
	Section     section = Section::Unknown;
	std::string line;
	while (std::getline(config_file, line)) {
		const std::string_view trimmed = trim(line);
		if (trimmed.empty() || trimmed.front() == '#' || trimmed.front() == ';') {
			continue;
		}
		if (trimmed.front() == '[' && trimmed.back() == ']') {
			section = lookup(SECTIONS, trim(trimmed.substr(1, trimmed.size() - 2))).value_or(Section::Unknown);
			continue;
		}
		const std::size_t separator = trimmed.find('=');
		if (separator != std::string_view::npos) {
			apply_setting(config, section, trim(trimmed.substr(0, separator)), trim(trimmed.substr(separator + 1)));
		}
	}
	return config;
}
