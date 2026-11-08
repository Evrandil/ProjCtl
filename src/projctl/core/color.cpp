#include "projctl.hpp"

auto ProjCtl::paint(Style::Color color) const -> std::string_view {
	return use_colors ? color_code(color) : "";
}

auto ProjCtl::reset() const -> std::string_view {
	return use_colors ? Style::RESET : "";
}
