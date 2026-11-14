#include "projctl.hpp"

#include "config.hpp"
#include "render_helper.hpp"
#include "row.hpp"
#include "style.hpp"
#include "text.hpp"

#include <format>
#include <iterator>
#include <ranges>
#include <string>
#include <string_view>
#include <vector>

auto ProjCtl::render_row(const Row& row, const std::vector<TableField>& fields) const -> std::string {
	std::string               output;
	std::back_insert_iterator output_inserter = std::back_inserter(output);
	const auto                parse_line =
		[&](const std::string_view header, const std::string_view text, const Style::Color color) -> void {
		std::format_to(
			output_inserter,
			"{}{}{:<{}}{}{}{}\n",
			Style::MARGIN,
			paint(config.colors.header),
			truncate(header, config.widths.name),
			config.widths.name,
			paint(color),
			text,
			reset()
		);
	};
	for (const TableField field : fields) {
		parse_line(RenderHelper::field_name(field), RenderHelper::cell_text(row, field), [&] -> Style::Color {
			if (field == TableField::State) {
				return RenderHelper::is_open(row.state) ? config.colors.open : config.colors.closed;
			}
			return column_of(config.tables, field).color;
		}());
	}
	if (row.body.empty()) {
		return output;
	}

	std::format_to(output_inserter, "\n");
	parse_line("Body", "", Style::Color::Default);
	for (const auto part : row.body | std::views::split('\n')) {
		std::format_to(
			output_inserter,
			"{}{}{}{}\n",
			Style::MARGIN,
			paint(config.tables.body.color),
			trim(std::string_view{part}),
			reset()
		);
	}
	return output;
}
