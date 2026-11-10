#include "projctl.hpp"

#include "config.hpp"
#include "row.hpp"
#include "style.hpp"
#include "table_helper.hpp"
#include "text.hpp"

#include <format>
#include <iterator>
#include <string>
#include <vector>

auto ProjCtl::render_table(const std::vector<Row>& rows, const std::vector<TableField>& columns) const -> std::string {
	const auto state_color = [&](const std::string& state) -> Style::Color {
		return (state == "OPEN" || state == "opened") ? config.colors.open : config.colors.closed;
	};

	std::string table = std::string{paint(config.colors.header)};
	for (const TableField field : columns) {
		const Settings::Column& column = column_of(config.tables, field);
		std::format_to(std::back_inserter(table), "{:<{}}", TableHelper::field_name(field), column.width);
	}
	std::format_to(std::back_inserter(table), "{}\n", reset());
	for (const Row& row : rows) {
		for (const TableField field : columns) {
			const Settings::Column& column = column_of(config.tables, field);
			const Style::Color      color  = field == TableField::State ? state_color(row.state) : column.color;

			std::format_to(
				std::back_inserter(table),
				"{}{:<{}}{}",
				paint(color),
				truncate(TableHelper::cell_text(row, field), column.width - 1),
				column.width,
				reset()
			);
		}
		table += '\n';
	}
	return table;
}
