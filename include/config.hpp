#pragma once

#include <expected>
#include <filesystem>
#include <vector>
#include <cstdlib>
#include <string>

#include "issue.hpp"
#include "request.hpp"
#include "style.hpp"

struct Config {
	size_t max_width             = 300;
	size_t name_width            = 20;
	size_t path_width            = 50;
	size_t remote_width          = 50;
	size_t branch_width          = 15;
	size_t table_number_width    = 8;
	size_t table_title_width     = 40;
	size_t table_author_width    = 20;
	size_t table_state_width     = 12;
	size_t table_labels_width    = 30;
	size_t table_created_width   = 22;
	size_t table_updated_width   = 22;
	size_t table_parent_width    = 10;

	std::vector<IssueField> issue_columns{
		IssueField::Number,
		IssueField::Title,
		IssueField::State,
		IssueField::Author,
		IssueField::Labels,
	};
	std::vector<RequestField> request_columns{
		RequestField::Number,
		RequestField::Title,
		RequestField::Author,
	};

	bool colors_enabled = true;

	Style::Color header_color               = Style::Color::Cyan;
	Style::Color project_name_color         = Style::Color::BoldYellow;
	Style::Color project_path_color         = Style::Color::Magenta;
	Style::Color project_branch_color       = Style::Color::Cyan;
	Style::Color project_remote_color       = Style::Color::Blue;
	Style::Color project_type_color         = Style::Color::BoldMagenta;
	Style::Color project_short_remote_color = Style::Color::BoldBlue;
	Style::Color tick_color                 = Style::Color::BoldGreen;
	Style::Color cross_color                = Style::Color::BoldRed;
	Style::Color table_number_color       = Style::Color::BoldMagenta;
	Style::Color table_title_color        = Style::Color::White;
	Style::Color table_open_color         = Style::Color::BoldGreen;
	Style::Color table_closed_color       = Style::Color::BoldRed;
	Style::Color table_author_color       = Style::Color::Blue;
	Style::Color table_labels_color       = Style::Color::Magenta;
	Style::Color table_parent_color       = Style::Color::Cyan;
	Style::Color error_color                = Style::Color::BoldRed;
	Style::Color warning_color              = Style::Color::Yellow;

	std::string editor = "nvim";
};

[[nodiscard]] auto load_config(const std::filesystem::path& path) -> std::expected<Config, std::string>;
