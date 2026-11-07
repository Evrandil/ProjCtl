#include "projctl.hpp"

#include "gitinterface.hpp"
#include "githostinterface.hpp"


std::expected<std::string, std::string> ProjCtl::issue_close(const std::optional<std::string>& name, int issue_number) const
{
	std::expected project_option = project_find(name);
	if (!project_option)
	{
		return std::unexpected(project_option.error());
	}

	const ProjectContent& content                      = (*project_option)->second;
	std::expected<HostOption, std::string> host_option = GitInterface::host(content);
	if (!host_option)
	{
		return std::unexpected(std::format("{}Failed to get host {}{}\n", paint(config.error_color), host_option.error(), reset()));
	}
	HostOption host = *host_option;

	return GitHostInterface::issue_close(host, content.path.string(), issue_number);
}


int ProjCtl::handle(const Cmd::GH::IssueClose& cmd) const
{
	return report(issue_close(cmd.name, cmd.number));
}
