#include "projctl.hpp"

#include "githostinterface.hpp"


std::expected<std::string, std::string> ProjCtl::issue_close(const std::optional<std::string>& name, int issue_number) const
{
	std::expected project_option = project_find(name);
	if (!project_option)
	{
		return std::unexpected(project_option.error());
	}

	std::expected result = GitHostInterface::issue_close((*project_option)->second.path.string(), issue_number);
	if (!result)
	{
		return std::unexpected(std::format("{}{}Failed to Close issue {}: {}{}\n", paint(config.error_color), symbol(Style::Mark::Cross), issue_number, result.error(), reset()));
	}

	return *result;
}


int ProjCtl::handle(const Cmd::GH::IssueClose& cmd) const
{
	return report(issue_close(cmd.name, cmd.number));
}
