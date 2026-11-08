#include "config.hpp"
#include "issue.hpp"
#include "style.hpp"

#include <cstddef>
#include <cstdint>
#include <expected>
#include <filesystem>
#include <fstream>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include <array>
#include <algorithm>

namespace {

	auto trim(std::string_view text) -> std::string {
		const std::size_t first = text.find_first_not_of(" \t\r\n");

		if (first == std::string_view::npos) {
			return "";
		}

		const std::size_t last = text.find_last_not_of(" \t\r\n");

		return std::string(text.substr(first, last - first + 1));
	}

	auto parse_issue_fields(std::string_view value) -> std::vector<IssueField> {
		std::vector<IssueField> fields;

		std::size_t start = 0;

		while (start < value.size()) {
			const std::size_t comma = value.find(',', start);

			const std::string field =
				trim(value.substr(start, comma == std::string_view::npos ? value.size() - start : comma - start));

			if (field == "number") {
				fields.push_back(IssueField::Number);
			} else if (field == "title") {
				fields.push_back(IssueField::Title);
			} else if (field == "state") {
				fields.push_back(IssueField::State);
			} else if (field == "author") {
				fields.push_back(IssueField::Author);
			} else if (field == "labels") {
				fields.push_back(IssueField::Labels);
			} else if (field == "parent") {
				fields.push_back(IssueField::Parent);
			}

			if (comma == std::string_view::npos) {
				break;
			}
			start = comma + 1;
		}

		return fields;
	}

	auto color_from_string(std::string_view value) -> std::optional<Style::Color> {
		if (value == "default") {
			return Style::Color::Default;
		}
		if (value == "black") {
			return Style::Color::Black;
		}
		if (value == "red") {
			return Style::Color::Red;
		}
		if (value == "green") {
			return Style::Color::Green;
		}
		if (value == "yellow") {
			return Style::Color::Yellow;
		}
		if (value == "blue") {
			return Style::Color::Blue;
		}
		if (value == "magenta") {
			return Style::Color::Magenta;
		}
		if (value == "cyan") {
			return Style::Color::Cyan;
		}
		if (value == "white") {
			return Style::Color::White;
		}

		return std::nullopt;
	}

	template <typename T>
	using Field = std::pair<std::string_view, T Config::*>;

	constexpr std::array DISPLAY_WIDTHS{
		Field<size_t>{"max_width", &Config::max_width},
		Field<size_t>{"name_width", &Config::name_width},
		Field<size_t>{"path_width", &Config::path_width},
		Field<size_t>{"remote_width", &Config::remote_width},
		Field<size_t>{"branch_width", &Config::branch_width},
	};
	constexpr std::array TABLE_WIDTHS{
		Field<size_t>{"number_width", &Config::table_number_width},
		Field<size_t>{"title_width", &Config::table_title_width},
		Field<size_t>{"author_width", &Config::table_author_width},
		Field<size_t>{"state_width", &Config::table_state_width},
		Field<size_t>{"labels_width", &Config::table_labels_width},
		Field<size_t>{"created_width", &Config::table_created_width},
		Field<size_t>{"updated_width", &Config::table_updated_width},
		Field<size_t>{"parent_width", &Config::table_parent_width},
	};

	constexpr std::array COLORS{
		Field<Style::Color>{"header", &Config::header_color},
		Field<Style::Color>{"project_name", &Config::project_name_color},
		Field<Style::Color>{"project_path", &Config::project_path_color},
		Field<Style::Color>{"project_branch", &Config::project_branch_color},
		Field<Style::Color>{"project_remote", &Config::project_remote_color},
		Field<Style::Color>{"project_type", &Config::project_type_color},
		Field<Style::Color>{"project_short_remote", &Config::project_short_remote_color},
		Field<Style::Color>{"tick", &Config::tick_color},
		Field<Style::Color>{"cross", &Config::cross_color},
		Field<Style::Color>{"table_number", &Config::table_number_color},
		Field<Style::Color>{"table_title", &Config::table_title_color},
		Field<Style::Color>{"table_open", &Config::table_open_color},
		Field<Style::Color>{"table_closed", &Config::table_closed_color},
		Field<Style::Color>{"table_author", &Config::table_author_color},
		Field<Style::Color>{"table_labels", &Config::table_labels_color},
		Field<Style::Color>{"table_parent", &Config::table_parent_color},
		Field<Style::Color>{"error", &Config::error_color},
		Field<Style::Color>{"warning", &Config::warning_color},
	};

	template <typename T, size_t N>
	auto find_field(const std::array<Field<T>, N>& fields, std::string_view key) -> T Config::* {
		const auto found = std::ranges::find(fields, key, &Field<T>::first);
		if (found != fields.end()) {
			return found->second;
		}
		return nullptr;
	}

	enum class Section : uint8_t {
		General,
		Display,
		Tables,
		Colors,
		Unknown,
	};

	Section section_from_string(std::string_view section) {
		if (section == "General") {
			return Section::General;
		}
		if (section == "Display") {
			return Section::Display;
		}
		if (section == "Issues") {
			return Section::Tables;
		}
		if (section == "Colors") {
			return Section::Colors;
		}
		return Section::Unknown;
	}
} // namespace

auto load_config(const std::filesystem::path& path) -> std::expected<Config, std::string> {
	std::ifstream config_file(path);

	if (!config_file) {
		return std::unexpected("Failed to open config file: " + path.string());
	}

	Config      config;
	Section section = Section::Unknown;
	std::string line;
	while (std::getline(config_file, line)) {
		line = trim(line);
		if (line.empty()) {
			continue;
		}
		if (line.front() == '#' || line.front() == ';') {
			continue;
		}

		if (line.front() == '[' && line.back() == ']') {
			section = section_from_string(trim(std::string_view(line).substr(1, line.size() - 2)));
			continue;
		}

		const std::size_t separator = line.find('=');
		if (separator == std::string::npos) {
			continue;
		}

		const std::string key   = trim(std::string_view(line.substr(0, separator)));
		const std::string value = trim(std::string_view(line.substr(separator + 1)));

		switch (section) {
			case Section::General:
				if (key == "editor") {
					config.editor = value;
				}
				continue;

			case Section::Display:
				{
					const size_t width = std::stoul(value);
					size_t Config::* field = find_field(DISPLAY_WIDTHS, key);
					if (field != nullptr) {
						config.*field = width;
					}
					continue;
				}

			case Section::Tables:
				{
					if (key == "columns") {
						std::vector<IssueField> parsed = parse_issue_fields(value);
						if (!parsed.empty()) {
							config.issue_columns = std::move(parsed);
						}
					}
					const size_t width = std::stoul(value);
					size_t Config::* field = find_field(TABLE_WIDTHS, key);
					if (field != nullptr) {
						config.*field = width;
					}
					continue;
				}

			case Section::Colors:
				{
					if (key == "enabled") {
						config.colors_enabled = value == "true";
						continue;
					}
					const std::optional<Style::Color> color = color_from_string(value);
					Style::Color Config::* field = find_field(COLORS, key);
					if (color && field != nullptr) {
						config.*field = *color;
					}
				}

			default: continue;
		}
	}
	return config;
}
