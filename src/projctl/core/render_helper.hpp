#pragma once

#include "row.hpp"

#include <string>
#include <string_view>

namespace RenderHelper {
	auto field_name(TableField field) -> std::string_view;
	auto cell_text(const Row& row, TableField field) -> std::string;
	auto is_open(std::string_view state) -> bool;
} // namespace RenderHelper
