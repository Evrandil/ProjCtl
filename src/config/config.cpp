#include "config.hpp"
#include "field.hpp"
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
	template <typename T>
	using Named = std::pair<std::string_view, T>;
	template <typename Owner, typename T>
	using Field = Named<T Owner::*>;

	enum class Section : uint8_t {
		General,
		Widths,
		Tables,
		Colors,
		Unknown,
	};
	constexpr auto SECTIONS = std::to_array<Named<Section>>({
		std::pair{"General", Section::General},
		std::pair{"Display", Section::Widths},
		std::pair{"Tables", Section::Tables},
		std::pair{"Colors", Section::Colors},
	});
	constexpr auto WIDTHS = std::to_array<Field<Settings::Widths, size_t>>({
		{"max_width", &Settings::Widths::max_width},
		{"name_width", &Settings::Widths::name_width},
		{"path_width", &Settings::Widths::path_width},
		{"remote_width", &Settings::Widths::remote_width},
		{"branch_width", &Settings::Widths::branch_width},
	});
	constexpr auto TABLE_WIDTHS   = std::to_array<Field<Settings::Tables::TableStyle, Settings::Column>>({
		{"number_width", &Settings::Tables::TableStyle::number},
		{"title_width", &Settings::Tables::TableStyle::title},
		{"author_width", &Settings::Tables::TableStyle::author},
		{"state_width", &Settings::Tables::TableStyle::state},
		{"labels_width", &Settings::Tables::TableStyle::labels},
		{"created_width", &Settings::Tables::TableStyle::created},
		{"updated_width", &Settings::Tables::TableStyle::updated},
		{"parent_width", &Settings::Tables::TableStyle::parent},
	});

	constexpr auto COLORS = std::to_array<Field<Settings::Colors, Style::Color>>({
		{"header", &Settings::Colors::header_color},
		{"project_name", &Settings::Colors::project_name_color},
		{"project_path", &Settings::Colors::project_path_color},
		{"project_branch", &Settings::Colors::project_branch_color},
		{"project_remote", &Settings::Colors::project_remote_color},
		{"project_type", &Settings::Colors::project_type_color},
		{"project_short_remote", &Settings::Colors::project_short_remote_color},
		{"tick", &Settings::Colors::tick_color},
		{"cross", &Settings::Colors::cross_color},
		{"error", &Settings::Colors::error_color},
		{"warning", &Settings::Colors::warning_color},
	});

	constexpr auto COLOR_NAMES = std::to_array<Named<Style::Color>>({
		{"default", Style::Color::Default},
		{"black", Style::Color::Black},
		{"boldBlack", Style::Color::BoldBlack},
		{"red", Style::Color::Red},
		{"boldRed", Style::Color::BoldRed},
		{"green", Style::Color::Green},
		{"boldGreen", Style::Color::BoldGreen},
		{"yellow", Style::Color::Yellow},
		{"boldYellow", Style::Color::BoldYellow},
		{"blue", Style::Color::Blue},
		{"boldBlue", Style::Color::BoldBlue},
		{"magenta", Style::Color::Magenta},
		{"boldMagenta", Style::Color::BoldMagenta},
		{"cyan", Style::Color::Cyan},
		{"boldCyan", Style::Color::BoldCyan},
		{"white", Style::Color::White},
		{"boldWhite", Style::Color::BoldWhite},
	});

	template <typename T, size_t N>
	constexpr auto lookup(const std::array<Named<T>, N>& table, std::string_view name) -> std::optional<T> {
		const auto* const found = std::ranges::find(table, name, &Named<T>::first);
		if (found == table.end()) {
			return std::nullopt;
		}
		return found->second;
	}

	auto trim(std::string_view text) -> std::string_view {
		const std::size_t first = text.find_first_not_of(" \t\r\n");

		if (first == std::string_view::npos) {
			return "";
		}

		const std::size_t last = text.find_last_not_of(" \t\r\n");

		return text.substr(first, last - first + 1);
	}

	auto parse_size(std::string_view text) -> std::optional<std::size_t> {
		std::size_t       value = 0;
		const char* const last  = std::to_address(text.end());
		const auto [end, error] = std::from_chars(std::to_address(text.begin()), last, value);
		if (error != std::errc{} || end != last) {
			return std::nullopt;
		}
		return value;
	}

	auto set_width(Settings::Widths& widths, std::string_view key, std::string_view value) -> void {
		const std::optional field = lookup(WIDTHS, key);
		const std::optional           width = parse_size(value);
		if (field && width) {
			widths.*(*field) = *width;
		}
	}

	auto parse_table_fields(std::string_view value) -> std::vector<TableField> {
		std::vector<TableField> fields;

		std::size_t start = 0;

		while (start < value.size()) {
			const std::size_t comma = value.find(',', start);

			const std::string_view field =
				trim(value.substr(start, comma == std::string_view::npos ? value.size() - start : comma - start));

			if (field == "number") {
				fields.push_back(TableField::Number);
			} else if (field == "title") {
				fields.push_back(TableField::Title);
			} else if (field == "state") {
				fields.push_back(TableField::State);
			} else if (field == "author") {
				fields.push_back(TableField::Author);
			} else if (field == "labels") {
				fields.push_back(TableField::Labels);
			} else if (field == "parent") {
				fields.push_back(TableField::Parent);
			}

			if (comma == std::string_view::npos) {
				break;
			}
			start = comma + 1;
		}

		return fields;
	}

	auto set_color(Settings::Colors& colors, std::string_view key, std::string_view value) -> void {
		if (key == "enabled") {
			colors.colors_enabled = value == "true";
			return;
		}
		const std::optional field = lookup(COLORS, key);
		const std::optional color = lookup(COLOR_NAMES, value);
		if (field && color) {
			colors.*(*field) = *color;
		}
	}

	auto set_table(Config& config, std::string_view key, std::string_view value) -> void {
		if (key != "columns") {
			set_width(config, TABLE_WIDTHS, key, value);
			return;
		}
		std::vector<IssueField> parsed = parse_table_fields(value);
		if (!parsed.empty()) {
			config.issue_columns = std::move(parsed);
		}
	}

	auto apply_setting(Config& config, const Section section, std::string_view key, std::string_view value) -> void {
		switch (section) {
			case Section::General:
				if (key == "editor") {
					config.editor = value;
				}
				return;

			case Section::Display: set_width(config, DISPLAY_WIDTHS, key, value); return;

			case Section::Tables: set_table(config, key, value); return;

			case Section::Colors:  set_color(config, key, value); return;
			case Section::Unknown: return;
		}
	}
} // namespace

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
			section = lookup(SECTIONS, trimmed.substr(1, trimmed.size() - 2)).value_or(Section::Unknown);
			continue;
		}

		const std::size_t separator = trimmed.find('=');
		if (separator == std::string::npos) {
			continue;
		}

		const std::string_view key   = trim(trimmed.substr(0, separator));
		const std::string_view value = trim(trimmed.substr(separator + 1));

		apply_setting(config, section, key, value);
	}
	return config;
}
