#include "projctl.hpp"

#include "gitinterface.hpp"


std::expected<std::string, std::string> ProjCtl::branch(const std::optional<std::string>& name, const std::optional<std::string>& branch_name) const
{
	std::expected project = project_find(name);
	if (!project)
	{
		return std::unexpected(project.error());
	}
	const ProjectContent& content = (*project)->second;

	if (!branch_name)
	{
		std::expected result = GitInterface::branch(content);
		if (!result)
		{
			return std::unexpected(std::format("{}{}{}", color_code(config.error_color), result.error(), Style::RESET));
		}

		return std::format("{}{}{}", color_code(config.project_branch_color), *result, Style::RESET);
	}

	std::expected result = GitInterface::branch_switch(content, *branch_name);
	if (!result)
	{
		return std::unexpected(std::format("{}Couldn't switch branches for {}{}{}: {}{}", color_code(config.error_color), color_code(config.project_name_color), (*project)->first, color_code(config.error_color), result.error(), Style::RESET));
	}
	return std::format("{}{}{}{}", color_code(config.tick_color), Style::TICK, *result, Style::RESET);
}
