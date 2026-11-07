#include "projctl.hpp"

#include "gitinterface.hpp"
#include "githostinterface.hpp"


std::expected<int, std::string> ProjCtl::issue_create(const std::optional<std::string>& name) const
{
	std::expected project_option = project_find(name);
	if (!project_option)
	{
		return std::unexpected(std::format("{}{}{}{}", paint(config.error_color), Style::CROSS, project_option.error(), reset()));
	}

	const ProjectContent& content                      = (*project_option)->second;
	std::expected host_option = GitInterface::host(content.path);
	if (!host_option)
	{
		return std::unexpected(std::format("{}Failed to get host {}{}\n", paint(config.error_color), host_option.error(), reset()));
	}
	HostOption host = *host_option;

	return GitHostInterface::issue_create(host, content.path.string());
}


int ProjCtl::handle(const Cmd::GH::IssueCreate& cmd) const
{
	return report(issue_create(cmd.name));
}
