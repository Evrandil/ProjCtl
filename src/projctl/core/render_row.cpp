#include "projctl.hpp"

#include "config.hpp"
#include "render_helper.hpp"
#include "row.hpp"
#include "style.hpp"
#include "text.hpp"

#include <format>
#include <iterator>
#include <string>
#include <string_view>
#include <vector>

auto ProjCtl::render_row(const Row& row, const std::vector<TableField>& fields) const -> std::string {
	std::string output;
	const auto  parse_line =
		[&](const std::string_view header, const std::string_view text, const Style::Color color) -> void {
		std::format_to(
			std::back_inserter(output),
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
				return row.state == "OPEN" || row.state == "opened" ? config.colors.open : config.colors.closed;
			}
			return column_of(config.tables, field).color;
		}());
	}
	std::format_to(
		std::back_inserter(output),
		"\n{}{}Body\n{}{}{}",
		Style::MARGIN,
		paint(config.colors.header),
		paint(config.tables.body.color),
		row.body,
		reset()
	);
	return output;
}
