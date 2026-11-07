#include "projctl.hpp"

#include "gitinterface.hpp"
#include "githostinterface.hpp"


std::expected<std::string, std::string> ProjCtl::request_merge(const std::optional<std::string>& name) const
{
	std::expected project = project_find(name);
	if (!project)
	{
		return std::unexpected(project.error());
	}

	const ProjectContent& content = (*project)->second;
	std::expected host_option = GitInterface::host(content.path);
	if (!host_option)
	{
		return std::unexpected(std::format("{}{}Failed to get host {}{}", paint(config.error_color), symbol(Style::Mark::Cross), host_option.error(), reset()));
	}
	HostOption host = *host_option;

	std::expected result = GitHostInterface::request_merge(host, content.path.string());
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
