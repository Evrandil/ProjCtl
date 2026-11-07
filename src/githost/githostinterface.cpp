#include "githostinterface.hpp"

#include "githubinterface.hpp"
#include "gitlabinterface.hpp"


CliState GitHostInterface::state(HostOption host)
{
	switch (host)
	{
		case HostOption::GitHub:
			return GitHubInterface::state();
		case HostOption::GitLab:
			return GitLabInterface::state();
		default:
			return CliState::NotInstalled;
	}
}


std::expected<std::string, std::string> GitHostInterface::repo_list(HostOption host)
{
	switch (host)
	{
		case HostOption::GitHub:
			return GitHubInterface::repo_list();
		case HostOption::GitLab:
			return GitLabInterface::repo_list();
		default:
			return std::unexpected("Unknown host");
	}
}
