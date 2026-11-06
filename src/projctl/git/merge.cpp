#include "projctl.hpp"

#include "gitinterface.hpp"


std::expected<std::string, std::string> ProjCtl::git_merge(const std::optional<std::string>& name) const
{
	std::expected project = project_find(name);
	if (!project)
	{
		return std::unexpected(std::format("{}{}{}{}", color_code(config.error_color), Style::CROSS, project.error(), Style::RESET));
	}
	const ProjectContent& content = (*project)->second;

	std::expected<std::string, std::string> result = GitInterface::default_merge(content);
	if (!result)
	{
		return std::unexpected(std::format("{}{}{}{}", color_code(config.error_color), Style::CROSS, result.error(), Style::RESET));
	}

	return std::format("{}{}{}{}", color_code(config.tick_color), Style::TICK, *result, Style::RESET);
}


int ProjCtl::handle(const Cmd::Git::Merge& cmd) const
{
	return report(git_merge(cmd.name));
}
