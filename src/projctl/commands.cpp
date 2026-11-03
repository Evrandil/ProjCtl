#include "projctl.hpp"

#include <print>


int ProjCtl::handle(const Cmd::Unknown& cmd) const
{
	std::println("\n\n{}Unrecognized command{}\n\n", color_code(config.error_color), Style::RESET);
	return 1;
}


int ProjCtl::handle(const Cmd::List& cmd) const
{
	std::expected<std::string, std::string> result;

	if (cmd.gits_only) { result = list_gits(); }
	else
	{
		result = list();
	}

	if (!result)
	{
		std::println("{}{}{}{}", color_code(config.error_color), Style::CROSS, result.error(), Style::RESET);
		return 1;
	}

	std::println("\n\n{}\n\n", *result);
	return 0;
}
