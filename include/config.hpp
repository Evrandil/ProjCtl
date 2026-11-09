#pragma once

#include <cstddef>
#include <expected>
#include <filesystem>
#include <string>
#include <vector>

#include "row.hpp"
#include "style.hpp"

namespace Settings {
	struct General {
		std::string editor = "nvim";
	};

	struct Widths {
		std::size_t max    = 300;
		std::size_t name   = 20;
		std::size_t path   = 50;
		std::size_t remote = 50;
		std::size_t branch = 15;
	};

	struct Column {
		std::size_t  width;
		Style::Color color;
	};

	struct Tables {
		Column number{.width = 8, .color = Style::Color::BoldMagenta};
		Column title{.width = 40, .color = Style::Color::White};
		Column state{.width = 12, .color = Style::Color::Default};
		Column author{.width = 20, .color = Style::Color::Blue};
		Column labels{.width = 30, .color = Style::Color::Magenta};
		Column created{.width = 22, .color = Style::Color::Default};
		Column updated{.width = 22, .color = Style::Color::Default};
		Column parent{.width = 10, .color = Style::Color::Cyan};

		std::vector<TableField> issue_columns{
			TableField::Number,
			TableField::Title,
			TableField::State,
			TableField::Author,
			TableField::Labels,
		};
		std::vector<TableField> request_columns{
			TableField::Number,
			TableField::Title,
			TableField::Author,
		};
	};

	struct Colors {
		bool enabled = true;

		Style::Color header       = Style::Color::Cyan;
		Style::Color name         = Style::Color::BoldYellow;
		Style::Color path         = Style::Color::Magenta;
		Style::Color branch       = Style::Color::Cyan;
		Style::Color remote       = Style::Color::Blue;
		Style::Color type         = Style::Color::BoldMagenta;
		Style::Color short_remote = Style::Color::BoldBlue;

		Style::Color tick    = Style::Color::BoldGreen;
		Style::Color cross   = Style::Color::BoldRed;
		Style::Color error   = Style::Color::BoldRed;
		Style::Color warning = Style::Color::Yellow;

		Style::Color open   = Style::Color::BoldGreen;
		Style::Color closed = Style::Color::BoldRed;
	};
} // namespace Settings

struct Config {
	Settings::General general;
	Settings::Widths  widths;
	Settings::Tables  tables;
	Settings::Colors  colors;
};

[[nodiscard]] auto column_of(const Settings::Tables& tables, TableField field) -> const Settings::Column&;
[[nodiscard]] auto load_config(const std::filesystem::path& path) -> std::expected<Config, std::string>;
