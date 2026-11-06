#include "projctl.hpp"

#include "gitinterface.hpp"


std::expected<std::string, std::string> ProjCtl::git_commit(const std::optional<std::string>& name, std::optional<std::string_view> message) const
{
	std::expected project = project_find(name);
	if (!project)
	{
		return std::unexpected(std::format("{}{}{}{}", color_code(config.error_color), Style::CROSS, project.error(), Style::RESET));
	}
	const ProjectContent& content = (*project)->second;

	std::expected<std::string, std::string> result;
	result = GitInterface::commit(content, message);

	if (!result)
	{
		return std::unexpected(std::format("{}{}{}{}", color_code(config.error_color), Style::CROSS, result.error(), Style::RESET));
	}
	return std::format("{}{}{}{}", color_code(config.tick_color), Style::TICK, *result, Style::RESET);
}


int ProjCtl::handle(const Cmd::Git::Commit& cmd) const
{
	return report(git_commit(cmd.name, cmd.message));
}
