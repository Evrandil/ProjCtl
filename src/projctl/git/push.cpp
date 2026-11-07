#include "projctl.hpp"

#include "gitinterface.hpp"



std::expected<std::string, std::string> ProjCtl::push(const std::optional<std::string>& name, const bool upstream) const
{
	std::expected project = project_find(name);
	if (!project)
	{
		return std::unexpected(project.error());
	}

	std::expected result = GitInterface::push((*project)->second.path, upstream);
	if (!result)
	{
		return std::unexpected(std::format("{}Couldn't push {}{}{}: {}{}", paint(config.error_color), paint(config.project_name_color), (*project)->first, paint(config.error_color), result.error(), reset()));
	}
	return std::format("{}{}{}{}", paint(config.tick_color), Style::TICK, *result, reset());
}


int ProjCtl::handle(const Cmd::Git::Push& cmd) const
{
	return report(push(cmd.name, cmd.branch_upstream));
}
