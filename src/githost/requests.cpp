#include "githostinterface.hpp"

#include "githostoption.hpp"
#include "githubinterface.hpp"
#include "gitinterface.hpp"
#include "gitlabinterface.hpp"
#include "row.hpp"
#include <expected>
#include <filesystem>
#include <string>
#include <vector>

auto GitHostInterface::request_list(const std::filesystem::path& path)
	-> std::expected<std::vector<Row>, std::string> {
	std::expected host = GitInterface::host(path);
	if (!host) {
		return std::unexpected(host.error());
	}
	switch (*host) {
		case HostOption::GitHub: return GitHubInterface::request_list(path);
		case HostOption::GitLab: return GitLabInterface::request_list(path);
		default:                 return std::unexpected("Unknown host");
	}
}

auto GitHostInterface::request_create(const std::filesystem::path& path) -> std::expected<int, std::string> {
	std::expected host = GitInterface::host(path);
	if (!host) {
		return std::unexpected(host.error());
	}
	switch (*host) {
		case HostOption::GitHub: return GitHubInterface::request_create(path);
		case HostOption::GitLab: return GitLabInterface::request_create(path);
		default:                 return std::unexpected("Unknown host");
	}
}

auto GitHostInterface::request_edit(const std::filesystem::path& path, int request_number)
	-> std::expected<int, std::string> {
	std::expected host = GitInterface::host(path);
	if (!host) {
		return std::unexpected(host.error());
	}
	switch (*host) {
		case HostOption::GitHub: return GitHubInterface::request_edit(path, request_number);
		case HostOption::GitLab: return GitLabInterface::request_edit(path, request_number);
		default:                 return std::unexpected("Unknown host");
	}
}

auto GitHostInterface::request_view(const std::filesystem::path& path, int request_number)
	-> std::expected<std::string, std::string> {
	std::expected host = GitInterface::host(path);
	if (!host) {
		return std::unexpected(host.error());
	}
	switch (*host) {
		case HostOption::GitHub: return GitHubInterface::request_view(path, request_number);
		case HostOption::GitLab: return GitLabInterface::request_view(path, request_number);
		default:                 return std::unexpected("Unknown host");
	}
}

auto GitHostInterface::request_close(const std::filesystem::path& path, int request_number)
	-> std::expected<std::string, std::string> {
	std::expected host = GitInterface::host(path);
	if (!host) {
		return std::unexpected(host.error());
	}
	switch (*host) {
		case HostOption::GitHub: return GitHubInterface::request_close(path, request_number);
		case HostOption::GitLab: return GitLabInterface::request_close(path, request_number);
		default:                 return std::unexpected("Unknown host");
	}
}

auto GitHostInterface::request_merge(const std::filesystem::path& path) -> std::expected<std::string, std::string> {
	std::expected host = GitInterface::host(path);
	if (!host) {
		return std::unexpected(host.error());
	}
	switch (*host) {
		case HostOption::GitHub: return GitHubInterface::request_merge(path);
		case HostOption::GitLab: return GitLabInterface::request_merge(path);
		default:                 return std::unexpected("Unknown host");
	}
}
