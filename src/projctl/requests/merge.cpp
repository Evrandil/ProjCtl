#include "projctl.hpp"

#include "githostinterface.hpp"


std::expected<std::string, std::string> ProjCtl::request_merge(const std::optional<std::string>& name) const
{
	std::expected project = project_find(name);
	if (!project)
	{
		return std::unexpected(project.error());
	}

	std::expected result = GitHostInterface::request_merge((*project)->second.path.string());
	if (!result)
	{
		return std::unexpected(std::format("{}{}{}{}", paint(config.error_color), symbol(Style::Mark::Cross), result.error(), reset()));
	}
	return std::format("{}{}{}{}", paint(config.tick_color), Style::TICK, *result, reset());
}


int ProjCtl::handle(const Cmd::GH::RequestMerge& cmd) const
{
	return report(request_merge(cmd.name));
}
