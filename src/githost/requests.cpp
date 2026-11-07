#include "githostinterface.hpp"

#include "gitinterface.hpp"
#include "githubinterface.hpp"
#include "gitlabinterface.hpp"


std::expected<std::vector<Request>, std::string> GitHostInterface::request_list(const std::filesystem::path& path)
{
	std::expected host = GitInterface::host(path);
	if (!host)
	{
		return std::unexpected(host.error());
	}
	switch (*host)
	{
		case HostOption::GitHub:
			return GitHubInterface::request_list(path);
		case HostOption::GitLab:
			return GitLabInterface::request_list(path);
		default:
			return std::unexpected("Unknown host");
	}
}


std::expected<int, std::string> GitHostInterface::request_create(const std::filesystem::path& path)
{
	std::expected host = GitInterface::host(path);
	if (!host)
	{
		return std::unexpected(host.error());
	}
	switch (*host)
	{
		case HostOption::GitHub:
			return GitHubInterface::request_create(path);
		case HostOption::GitLab:
			return GitLabInterface::request_create(path);
		default:
			return std::unexpected("Unknown host");
	}
}


std::expected<int, std::string> GitHostInterface::request_edit(const std::filesystem::path& path, int request_number)
{
	std::expected host = GitInterface::host(path);
	if (!host)
	{
		return std::unexpected(host.error());
	}
	switch (*host)
	{
		case HostOption::GitHub:
			return GitHubInterface::request_edit(path, request_number);
		case HostOption::GitLab:
			return GitLabInterface::request_edit(path, request_number);
		default:
			return std::unexpected("Unknown host");
	}
}

std::expected<std::string, std::string> GitHostInterface::request_close(const std::filesystem::path& path, int request_number)
{
	std::expected host = GitInterface::host(path);
	if (!host)
	{
		return std::unexpected(host.error());
	}
	switch (*host)
	{
		case HostOption::GitHub:
			return GitHubInterface::request_close(path, request_number);
		case HostOption::GitLab:
			return GitLabInterface::request_close(path, request_number);
		default:
			return std::unexpected("Unknown host");
	}
}


std::expected<std::string, std::string> GitHostInterface::request_merge(const std::filesystem::path& path)
{
	std::expected host = GitInterface::host(path);
	if (!host)
	{
		return std::unexpected(host.error());
	}
	switch (*host)
	{
		case HostOption::GitHub:
			return GitHubInterface::request_merge(path);
		case HostOption::GitLab:
			return GitLabInterface::request_merge(path);
		default:
			return std::unexpected("Unknown host");
	}
}
