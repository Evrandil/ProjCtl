#include "config.hpp"
#include "issue.hpp"
#include "style.hpp"

#include <cstddef>
#include <expected>
#include <filesystem>
#include <fstream>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace {

	auto trim(std::string_view text) -> std::string {
		std::size_t first = text.find_first_not_of(" \t\r\n");

		if (first == std::string_view::npos) {
			return "";
		}

		std::size_t last = text.find_last_not_of(" \t\r\n");

		return std::string(text.substr(first, last - first + 1));
	}

	auto parse_issue_fields(std::string_view value) -> std::vector<IssueField> {
		std::vector<IssueField> fields;

		std::size_t start = 0;

		while (start < value.size()) {
			std::size_t comma = value.find(',', start);

			std::string field =
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

} // namespace

// TODO Make this load func more readable ?
auto load_config(const std::filesystem::path& path) -> std::expected<Config, std::string> {
	std::ifstream config_file(path);

	if (!config_file) {
		return std::unexpected("Failed to open config file: " + path.string());
	}

	Config      config;
	std::string section; // TODO change to enum so we can use switch statement instead of if else chain ?
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
			section = trim(std::string_view(line).substr(1, line.size() - 2));
			continue;
		}

		std::size_t separator = line.find('=');
		if (separator == std::string::npos) {
			continue;
		}

		std::string key   = trim(std::string_view(line.substr(0, separator)));
		std::string value = trim(std::string_view(line.substr(separator + 1)));

		if (section == "General") {
			if (key == "editor") {
				config.editor = value;
			}
			continue;
		}

		if (section == "Display") {
			if (key == "name_width") {
				config.name_width = std::stoul(value);
			} else if (key == "max_width") {
				config.max_width = std::stoul(value);
			} else if (key == "path_width") {
				config.path_width = std::stoul(value);
			} else if (key == "remote_width") {
				config.remote_width = std::stoul(value);
			} else if (key == "branch_width") {
				config.branch_width = std::stoul(value);
			}
			continue;
		}

		if (section == "Issues") {
			if (key == "number_width") {
				config.issue_number_width = std::stoul(value);
			} else if (key == "title_width") {
				config.issue_title_width = std::stoul(value);
			} else if (key == "author_width") {
				config.issue_author_width = std::stoul(value);
			} else if (key == "state_width") {
				config.issue_state_width = std::stoul(value);
			} else if (key == "labels_width") {
				config.issue_labels_width = std::stoul(value);
			} else if (key == "columns") {
				std::vector<IssueField> parsed = parse_issue_fields(value);
				if (!parsed.empty()) {
					config.issue_columns = std::move(parsed);
				}
			}
			continue;
		}

		if (section == "Colors") {
			if (key == "enabled") {
				config.colors_enabled = value == "true";
				continue;
			}
			std::optional<Style::Color> color = color_from_string(value);
			if (color) {
				if (key == "issue_number") {
					config.issue_number_color = *color;
				} else if (key == "header_color") {
					config.header_color = *color;
				} else if (key == "issue_title_color") {
					config.issue_title_color = *color;
				} else if (key == "issue_open_color") {
					config.issue_open_color = *color;
				} else if (key == "issue_closed_color") {
					config.issue_closed_color = *color;
				} else if (key == "issue_author_color") {
					config.issue_author_color = *color;
				} else if (key == "issue_labels_color") {
					config.issue_labels_color = *color;
				} else if (key == "issue_parent_color") {
					config.issue_parent_color = *color;
				} else if (key == "error_color") {
					config.error_color = *color;
				} else if (key == "warning_color") {
					config.warning_color = *color;
				} else if (key == "project_name_color") {
					config.project_name_color = *color;
				} else if (key == "project_path_color") {
					config.project_path_color = *color;
				} else if (key == "project_branch_color") {
					config.project_branch_color = *color;
				} else if (key == "project_remote_color") {
					config.project_remote_color = *color;
				} else if (key == "project_type_color") {
					config.project_type_color = *color;
				} else if (key == "project_short_remote_color") {
					config.project_short_remote_color = *color;
				} else if (key == "tick_color") {
					config.tick_color = *color;
				} else if (key == "cross_color") {
					config.cross_color = *color;
				}
			}
		}
	}
	return config;
}
