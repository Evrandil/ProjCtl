#pragma once

#include <cstdlib>
#include <expected>
#include <filesystem>
#include <string>
#include <vector>

#include "field.hpp"
#include "style.hpp"

namespace Settings {
	struct General {
		std::string editor = "nvim";
	};

	struct Widths {
		size_t max_width    = 300;
		size_t name_width   = 20;
		size_t path_width   = 50;
		size_t remote_width = 50;
		size_t branch_width = 15;
	};

	struct Column {
		std::size_t  width;
		Style::Color color;
	};

	struct Tables {
		struct TableStyle {
			Column number{.width = 8, .color = Style::Color::BoldMagenta};
			Column title{.width = 40, .color = Style::Color::White};
			Column state{.width = 12, .color = Style::Color::Default};
			Column author{.width = 20, .color = Style::Color::Blue};
			Column labels{.width = 30, .color = Style::Color::Magenta};
			Column created{.width = 22, .color = Style::Color::Default};
			Column updated{.width = 22, .color = Style::Color::Default};
			Column parent{.width = 10, .color = Style::Color::Cyan};
		};

		std::vector<TableField> table_columns{
			TableField::Number,
			TableField::Title,
			TableField::State,
			TableField::Author,
			TableField::Labels,
		};
	};

	struct Colors {
		bool colors_enabled = true;

		Style::Color header_color               = Style::Color::Cyan;
		Style::Color project_name_color         = Style::Color::BoldYellow;
		Style::Color project_path_color         = Style::Color::Magenta;
		Style::Color project_branch_color       = Style::Color::Cyan;
		Style::Color project_remote_color       = Style::Color::Blue;
		Style::Color project_type_color         = Style::Color::BoldMagenta;
		Style::Color project_short_remote_color = Style::Color::BoldBlue;

		Style::Color open          = Style::Color::BoldGreen;
		Style::Color closed        = Style::Color::BoldRed;
		Style::Color error_color   = Style::Color::BoldRed;
		Style::Color warning_color = Style::Color::Yellow;
		Style::Color tick_color    = Style::Color::BoldGreen;
		Style::Color cross_color   = Style::Color::BoldRed;
	};
}; // namespace Settings

struct Config {

	Settings::General general;
	Settings::Widths  widths;
	Settings::Tables  tables;
	Settings::Colors  colors;
};

[[nodiscard]] auto column_of(const Settings::Tables& table, TableField field) -> const Settings::Column&;
[[nodiscard]] auto load_config(const std::filesystem::path& path) -> std::expected<Config, std::string>;
