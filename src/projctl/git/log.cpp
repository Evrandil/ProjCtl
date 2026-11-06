#include "projctl.hpp"

#include "gitinterface.hpp"


std::expected<std::string, std::string> ProjCtl::log(const std::optional<std::string>& name, bool oneline) const
{
	std::expected project = project_find(name);
	if (!project)
	{
		return std::unexpected(std::format("{}{}{}{}", color_code(config.error_color), Style::CROSS, project.error(), Style::RESET));
	}

	std::expected result = GitInterface::log((*project)->second, oneline);
	if (!result)
	{
		return std::unexpected(std::format("{}{}{}{}", color_code(config.error_color), Style::CROSS, result.error(), Style::RESET));
	}
	return std::format("{}{}{}{}", color_code(config.tick_color), Style::TICK, *result, Style::RESET);
}


int ProjCtl::handle(const Cmd::Git::Log& cmd) const
{
	return report(log(cmd.name, cmd.oneline));
}
