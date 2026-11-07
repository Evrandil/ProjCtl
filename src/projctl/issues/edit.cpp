#include "projctl.hpp"

#include "gitinterface.hpp"
#include "githostinterface.hpp"


std::expected<int, std::string> ProjCtl::issue_edit(const std::optional<std::string>& name, int issue_number) const
{
	std::expected project = project_find(name);
	if (!project)
	{
		return std::unexpected(project.error());
	}

	const ProjectContent& content                      = (*project)->second;
	std::expected host_option = GitInterface::host(content.path);
	if (!host_option)
	{
		return std::unexpected(std::format("{}{}Failed to get host {}{}\n", paint(config.error_color), symbol(Style::Mark::Cross), host_option.error(), reset()));
	}
	HostOption host = *host_option;

	return GitHostInterface::issue_edit(host, content.path.string(), issue_number);
}


int ProjCtl::handle(const Cmd::GH::IssueEdit& cmd) const
{
	return report(issue_edit(cmd.name, cmd.number));
}
