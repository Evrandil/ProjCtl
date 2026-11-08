#pragma once

#include <filesystem>
#include <vector>

#include "issue.hpp"
#include "request.hpp"
#include "style.hpp"

// TODO Turn this into class and make public getters and members private
struct Config {
	std::size_t max_width             = 300;
	std::size_t name_width            = 20;
	std::size_t path_width            = 50;
	std::size_t remote_width          = 50;
	std::size_t branch_width          = 15;
	std::size_t issue_number_width    = 8;
	std::size_t issue_title_width     = 40;
	std::size_t issue_author_width    = 20;
	std::size_t issue_state_width     = 12;
	std::size_t issue_labels_width    = 30;
	std::size_t issue_created_width   = 22;
	std::size_t issue_updated_width   = 22;
	std::size_t issue_parent_width    = 10;
	std::size_t request_number_width  = 8;
	std::size_t request_title_width   = 40;
	std::size_t request_author_width  = 20;
	std::size_t request_state_width   = 12;
	std::size_t request_labels_width  = 30;
	std::size_t request_created_width = 22;
	std::size_t request_updated_width = 22;
	std::size_t request_parent_width  = 10;

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
	Style::Color issue_number_color         = Style::Color::BoldMagenta;
	Style::Color issue_title_color          = Style::Color::White;
	Style::Color issue_open_color           = Style::Color::BoldGreen;
	Style::Color issue_closed_color         = Style::Color::BoldRed;
	Style::Color issue_author_color         = Style::Color::Blue;
	Style::Color issue_labels_color         = Style::Color::Magenta;
	Style::Color issue_parent_color         = Style::Color::Cyan;
	Style::Color request_number_color       = Style::Color::BoldMagenta;
	Style::Color request_title_color        = Style::Color::White;
	Style::Color request_open_color         = Style::Color::BoldGreen;
	Style::Color request_closed_color       = Style::Color::BoldRed;
	Style::Color request_author_color       = Style::Color::Blue;
	Style::Color request_labels_color       = Style::Color::Magenta;
	Style::Color request_parent_color       = Style::Color::Cyan;
	Style::Color error_color                = Style::Color::BoldRed;
	Style::Color warning_color              = Style::Color::Yellow;

	std::string editor = "nvim";

	void load(const std::filesystem::path& path);
};
