#include "projctl.hpp"

#include "gitinterface.hpp"


std::expected<std::string, std::string> ProjCtl::log(const std::optional<std::string>& name, bool oneline) const
{
	std::expected project = project_find(name);
	if (!project)
	{
		return std::unexpected(std::format("{}{}{}{}", paint(config.error_color), Style::CROSS, project.error(), reset()));
	}

	std::expected result = GitInterface::log((*project)->second.path, oneline);
	if (!result)
	{
		return std::unexpected(std::format("{}{}{}{}", paint(config.error_color), Style::CROSS, result.error(), reset()));
	}
	return std::format("{}{}{}{}", paint(config.tick_color), Style::TICK, *result, reset());
}


int ProjCtl::handle(const Cmd::Git::Log& cmd) const
{
	return report(log(cmd.name, cmd.oneline));
}
