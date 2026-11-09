#include "projctl.hpp"

#include "config.hpp"
#include "row.hpp"
#include "style.hpp"

#include <format>
#include <iterator>
#include <ranges>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace {
	auto cell_text(const Row& row, const TableField field) -> std::string {
		switch (field) {
			case TableField::Number: return std::format("#{}", row.number);
			case TableField::Title:  return row.title;
			case TableField::State:  return row.state;
			case TableField::Author: return row.author;
			case TableField::Labels:
				return row.labels | std::views::join_with(std::string_view{", "}) | std::ranges::to<std::string>();
			case TableField::Created: return row.created;
			case TableField::Updated: return row.updated;
			case TableField::Parent:  return row.parent.value_or("-");
		}
		std::unreachable();
	}

	auto field_name(const TableField field) -> std::string_view {
		switch (field) {
			case TableField::Number:  return "Number";
			case TableField::Title:   return "Title";
			case TableField::State:   return "State";
			case TableField::Author:  return "Author";
			case TableField::Labels:  return "Labels";
			case TableField::Created: return "Created";
			case TableField::Updated: return "Updated";
			case TableField::Parent:  return "Parent";
		}
		std::unreachable();
	}
} // namespace

auto ProjCtl::render_table(const std::vector<Row>& rows, const std::vector<TableField>& columns) const -> std::string {
	auto state_color = [&](const std::string& state) -> Style::Color {
		return (state == "OPEN" || state == "opened") ? config.colors.open : config.colors.closed;
	};

	std::string table = std::string{paint(config.colors.header)};
	for (const TableField field : columns) {
		const Settings::Column& column = column_of(config.tables, field);
		std::format_to(std::back_inserter(table), "{:<{}}", field_name(field), column.width);
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
				truncate(cell_text(row, field), column.width - 1),
				column.width,
				reset()
			);
		}
		table += '\n';
	}
	return table;
}
