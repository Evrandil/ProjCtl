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

	constexpr auto COLORS = std::to_array<Field<Style::Color>>({
		{"header", &Config::header_color},
		{"project_name", &Config::project_name_color},
		{"project_path", &Config::project_path_color},
		{"project_branch", &Config::project_branch_color},
		{"project_remote", &Config::project_remote_color},
		{"project_type", &Config::project_type_color},
		{"project_short_remote", &Config::project_short_remote_color},
		{"tick", &Config::tick_color},
		{"cross", &Config::cross_color},
		{"table_number", &Config::table_number_color},
		{"table_title", &Config::table_title_color},
		{"table_open", &Config::table_open_color},
		{"table_closed", &Config::table_closed_color},
		{"table_author", &Config::table_author_color},
		{"table_labels", &Config::table_labels_color},
		{"table_parent", &Config::table_parent_color},
		{"error", &Config::error_color},
		{"warning", &Config::warning_color},
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

	template <size_t N>
	auto
	set_width(Config& config, const std::array<Field<size_t>, N>& fields, std::string_view key, std::string_view value)
		-> void {
		const std::optional<size_t Config::*> field = lookup(fields, key);
		const std::optional<size_t>           width = parse_size(value);
		if (field && width) {
			config.** field = *width;
		}
	}

	auto parse_table_fields(std::string_view value) -> std::vector<IssueField> {
		std::vector<IssueField> fields;

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

	auto set_color(Config& config, std::string_view key, std::string_view value) -> void {
		if (key == "enabled") {
			config.colors_enabled = value == "true";
			return;
		}
		const std::optional field = lookup(COLORS, key);
		const std::optional color = lookup(COLOR_NAMES, value);
		if (field && color) {
			config.** field = *color;
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
