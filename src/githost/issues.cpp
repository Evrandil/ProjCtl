#include "githostinterface.hpp"

#include "githubinterface.hpp"
#include "gitlabinterface.hpp"


std::expected<std::vector<Issue>, std::string> GitHostInterface::issue_list(HostOption host, std::string_view path)
{
	switch (host)
	{
		case HostOption::GitHub:
			return GitHubInterface::issue_list(path);
		case HostOption::GitLab:
			return GitLabInterface::issue_list(path);
		default:
			return std::unexpected("Unknown host");
	}
}


std::expected<int, std::string> GitHostInterface::issue_create(HostOption host, std::string_view path)
{
	switch (host)
	{
		case HostOption::GitHub:
			return GitHubInterface::issue_create(path);
		case HostOption::GitLab:
			return GitLabInterface::issue_create(path);
		default:
			return std::unexpected("Unknown host");
	}
}


std::expected<int, std::string> GitHostInterface::issue_edit(HostOption host, std::string_view path, int issue_number)
{
	switch (host)
	{
		case HostOption::GitHub:
			return GitHubInterface::issue_edit(path, issue_number);
		case HostOption::GitLab:
			return GitLabInterface::issue_edit(path, issue_number);
		default:
			return std::unexpected("Unknown host");
	}
}


std::expected<std::string, std::string> GitHostInterface::issue_close(HostOption host, std::string_view path, int issue_number)
{
	switch (host)
	{
		case HostOption::GitHub:
			return GitHubInterface::issue_close(path, issue_number);
		case HostOption::GitLab:
			return GitLabInterface::issue_close(path, issue_number);
		default:
			return std::unexpected("Unknown host");
	}
}
