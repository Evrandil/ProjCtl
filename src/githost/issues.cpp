#include "githostinterface.hpp"

#include "gitinterface.hpp"
#include "githubinterface.hpp"
#include "gitlabinterface.hpp"


std::expected<std::vector<Issue>, std::string> GitHostInterface::issue_list(const std::filesystem::path& path)
{
	std::expected host = GitInterface::host(path);
	if (!host)
	{
		return std::unexpected(host.error());
	}
	switch (*host)
	{
		case HostOption::GitHub:
			return GitHubInterface::issue_list(path);
		case HostOption::GitLab:
			return GitLabInterface::issue_list(path);
		default:
			return std::unexpected("Unknown host");
	}
}


std::expected<int, std::string> GitHostInterface::issue_create(const std::filesystem::path& path)
{
	std::expected host = GitInterface::host(path);
	if (!host)
	{
		return std::unexpected(host.error());
	}
	switch (*host)
	{
		case HostOption::GitHub:
			return GitHubInterface::issue_create(path);
		case HostOption::GitLab:
			return GitLabInterface::issue_create(path);
		default:
			return std::unexpected("Unknown host");
	}
}


std::expected<int, std::string> GitHostInterface::issue_edit(const std::filesystem::path& path, int issue_number)
{
	std::expected host = GitInterface::host(path);
	if (!host)
	{
		return std::unexpected(host.error());
	}
	switch (*host)
	{
		case HostOption::GitHub:
			return GitHubInterface::issue_edit(path, issue_number);
		case HostOption::GitLab:
			return GitLabInterface::issue_edit(path, issue_number);
		default:
			return std::unexpected("Unknown host");
	}
}


std::expected<std::string, std::string> GitHostInterface::issue_view(const std::filesystem::path& path,
																	 int issue_number)
{
	std::expected host = GitInterface::host(path);
	if (!host)
	{
		return std::unexpected(host.error());
	}
	switch (*host)
	{
		case HostOption::GitHub:
			return GitHubInterface::issue_view(path, issue_number);
		case HostOption::GitLab:
			return GitLabInterface::issue_view(path, issue_number);
		default:
			return std::unexpected("Unknown host");
	}
}


std::expected<std::string, std::string> GitHostInterface::issue_close(const std::filesystem::path& path,
																	  int issue_number)
{
	std::expected host = GitInterface::host(path);
	if (!host)
	{
		return std::unexpected(host.error());
	}
	switch (*host)
	{
		case HostOption::GitHub:
			return GitHubInterface::issue_close(path, issue_number);
		case HostOption::GitLab:
			return GitLabInterface::issue_close(path, issue_number);
		default:
			return std::unexpected("Unknown host");
	}
}
