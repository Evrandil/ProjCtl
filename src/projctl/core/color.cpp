#include "projctl.hpp"


std::string_view ProjCtl::paint(Style::Color color) const
{
	return use_colors ? color_code(color) : "";
}

std::string_view ProjCtl::reset() const
{
	return use_colors ? Style::RESET : "";
}
