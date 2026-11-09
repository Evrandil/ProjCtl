#include "projctl.hpp"

#include "row.hpp"
#include "config.hpp"
#include "style.hpp"

#include <vector>
#include <string>
#include <string_view>
#include <format>
#include <iterator>
#include <ranges>
#include <utility>


namespace {
	auto cell_text(const Row& row, const TableRow& field) -> std::string {
		switch (field) {
			case TableRow::Number:  return std::format("#{}", row.number);
			case TableRow::Title:   return row.title;
			case TableRow::State:   return row.state;
			case TableRow::Author:  return row.author;
			case TableRow::Labels:  return row.labels | std::views::join_with(std::string_view{", "}) | std::ranges::to<std::string>();
			case TableRow::Created: return row.created;
			case TableRow::Updated: return row.updated;
			case TableRow::Parent:  return row.parent.value_or("-");
		}
		std::unreachable();
	}
	auto field_name(const TableRow field) -> std::string_view {
		switch (field) {
			case TableRow::Number:  return "Number";
			case TableRow::Title:   return "Title";
			case TableRow::State:   return "State";
			case TableRow::Author:  return "Author";
			case TableRow::Labels:  return "Labels";
			case TableRow::Created: return "Created";
			case TableRow::Updated: return "Updated";
			case TableRow::Parent:  return "Parent";
		}
		std::unreachable();
	}
}


auto ProjCtl::render_table(const std::vector<Row>& rows, const std::vector<TableRow>& columns) const -> std::string {
	auto state_color = [&](const std::string& state) -> Style::Color {
		return (state == "OPEN" || state == "opened") ? config.colors.open : config.colors.closed;
	};

	std::string table = std::format("{}\n", paint(config.colors.header));
	for (const TableRow field : columns) {
		const Settings::Column& column = column_of(config.tables, field);
		std::format_to(std::back_inserter(table), "{:<{}}", field_name(field), column.width);
	}
	std::format_to(std::back_inserter(table),"{}\n", reset());
	for (const Row& row : rows) {
		for (const TableRow& field : columns) {
			const Settings::Column& column = column_of(config.tables, field);
			const Style::Color color = field == TableRow::State ? state_color(row.state) : column.color;

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
