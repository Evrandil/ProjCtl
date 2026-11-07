#include "projctl.hpp"

#include "githostinterface.hpp"


std::expected<std::string, std::string> ProjCtl::issue_view(const std::optional<std::string>& name, int issue_number) const
{
	const std::expected project = project_find(name);
	if (!project)
	{
		return std::unexpected(project.error());
	}
	return GitHostInterface::issue_view((*project)->second.path, issue_number);
}


int ProjCtl::handle(const Cmd::GH::IssueView& cmd) const
{
	return report(issue_view(cmd.name, cmd.number));
}
