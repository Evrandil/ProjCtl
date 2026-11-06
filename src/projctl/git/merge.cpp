#include "projctl.hpp"

#include "gitinterface.hpp"


std::expected<std::string, std::string> ProjCtl::git_merge(const std::optional<std::string>& name) const
{
	std::expected project = project_find(name);
	if (!project)
	{
		return std::unexpected(std::format("{}{}{}{}", paint(config.error_color), Style::CROSS, project.error(), reset()));
	}
	const ProjectContent& content = (*project)->second;

	std::expected<std::string, std::string> result = GitInterface::default_merge(content);
	if (!result)
	{
		return std::unexpected(std::format("{}{}{}{}", paint(config.error_color), Style::CROSS, result.error(), reset()));
	}

	return std::format("{}{}{}{}", paint(config.tick_color), Style::TICK, *result, reset());
}


int ProjCtl::handle(const Cmd::Git::Merge& cmd) const
{
	return report(git_merge(cmd.name));
}
