#include "table_helper.hpp"

#include "row.hpp"

#include <format>
#include <ranges>
#include <string>
#include <string_view>
#include <utility>

auto TableHelper::cell_text(const Row& row, const TableField field) -> std::string {
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

auto TableHelper::field_name(const TableField field) -> std::string_view {
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
