#include "projctl.hpp"

#include "gitinterface.hpp"



std::expected<std::string, std::string> ProjCtl::push(const std::optional<std::string>& name, const bool upstream) const
{
	std::expected project = project_find(name);
	if (!project)
	{
		return std::unexpected(std::format("{}{}{}{}", color_code(config.error_color), Style::CROSS, project.error(), Style::RESET));
	}

	std::expected result = GitInterface::push((*project)->second, upstream);
	if (!result)
	{
		return std::unexpected(std::format("{}Couldn't push {}{}{}: {}{}", color_code(config.error_color), color_code(config.project_name_color), (*project)->first, color_code(config.error_color), result.error(), Style::RESET));
	}
	return std::format("{}{}{}{}", color_code(config.tick_color), Style::TICK, *result, Style::RESET);
}


int ProjCtl::handle(const Cmd::Git::Push& cmd) const
{
	return report(push(cmd.name, cmd.branch_upstream));
}
