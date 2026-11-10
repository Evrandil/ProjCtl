#pragma once

#include "row.hpp"

#include <string>
#include <string_view>


namespace TableHelper {
	auto field_name(TableField field) -> std::string_view;
	auto cell_text(const Row& row, TableField field) -> std::string;
}
