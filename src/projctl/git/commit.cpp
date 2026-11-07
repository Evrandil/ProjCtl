#include "projctl.hpp"

#include "gitinterface.hpp"


std::expected<std::string, std::string> ProjCtl::git_commit(const std::optional<std::string>& name, std::optional<std::string_view> message) const
{
	std::expected project = project_find(name);
	if (!project)
	{
		return std::unexpected(project.error());
	}
	const ProjectContent& content = (*project)->second;

	std::expected<std::string, std::string> result;
	result = GitInterface::commit(content.path, message);

	if (!result)
	{
		return std::unexpected(std::format("{}{}{}{}", paint(config.error_color), Style::CROSS, result.error(), reset()));
	}
	return std::format("{}{}{}{}", paint(config.tick_color), Style::TICK, *result, reset());
}


int ProjCtl::handle(const Cmd::Git::Commit& cmd) const
{
	return report(git_commit(cmd.name, cmd.message));
}
