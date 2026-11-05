#include "projctl.hpp"

#include "gitinterface.hpp"


std::expected<std::string, std::string> ProjCtl::git_merge(const std::optional<std::string>& name) const
{
	std::expected project = project_find(name);
	if (!project)
	{
		return std::unexpected(project.error());
	}
	const ProjectContent& content = (*project)->second;

	std::expected<std::string, std::string> result = GitInterface::default_merge(content);
	if (!result)
	{
		return std::unexpected(result.error());
	}

	return std::format("{}{}{}", color_code(config.tick_color), Style::TICK, *result);
}


int ProjCtl::handle(const Cmd::Git::Merge& cmd) const
{
	return report(git_merge(cmd.name));
}
