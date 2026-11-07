#include "projctl.hpp"

#include "githostinterface.hpp"


std::expected<int, std::string> ProjCtl::issue_create(const std::optional<std::string>& name) const
{
	std::expected project = project_find(name);
	if (!project)
	{
		return std::unexpected(project.error());
	}

	std::expected result = GitHostInterface::issue_create((*project)->second.path.string());
	if (!result)
	{
		return std::unexpected(std::format("{}{}Failed to create issue: {}{}\n", paint(config.error_color), symbol(Style::Mark::Cross), result.error(), reset()));
	}

	return *result;
}


int ProjCtl::handle(const Cmd::GH::IssueCreate& cmd) const
{
	return report(issue_create(cmd.name));
}
