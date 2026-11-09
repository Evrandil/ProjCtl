#include "githostinterface.hpp"

#include "githostoption.hpp"
#include "githubinterface.hpp"
#include "gitinterface.hpp"
#include "gitlabinterface.hpp"
#include "row.hpp"

#include <expected>
#include <filesystem>
#include <string>
#include <utility>
#include <vector>

namespace {
	template <typename GitHubCall, typename GitLabCall>
	auto on_host(const std::filesystem::path& path, GitHubCall github, GitLabCall gitlab) -> decltype(github()) {
		const std::expected host = GitInterface::host(path);
		if (!host) {
			return std::unexpected(host.error());
		}
		switch (*host) {
			case HostOption::GitHub:  return github();
			case HostOption::GitLab:  return gitlab();
			case HostOption::Unknown: return std::unexpected("Unknown host");
		}
		std::unreachable();
	}
} // namespace

auto GitHostInterface::issue_list(const std::filesystem::path& path) -> std::expected<std::vector<Row>, std::string> {
	return on_host(
		path,
		[&] { return GitHubInterface::issue_list(path); },
		[&] { return GitLabInterface::issue_list(path); }
	);
}

auto GitHostInterface::issue_create(const std::filesystem::path& path) -> std::expected<int, std::string> {
	return on_host(
		path,
		[&] { return GitHubInterface::issue_create(path); },
		[&] { return GitLabInterface::issue_create(path); }
	);
}

auto GitHostInterface::issue_edit(const std::filesystem::path& path, int issue_number)
	-> std::expected<int, std::string> {
	return on_host(
		path,
		[&] { return GitHubInterface::issue_edit(path, issue_number); },
		[&] { return GitLabInterface::issue_edit(path, issue_number); }
	);
}

auto GitHostInterface::issue_view(const std::filesystem::path& path, int issue_number)
	-> std::expected<std::string, std::string> {
	return on_host(
		path,
		[&] { return GitHubInterface::issue_view(path, issue_number); },
		[&] { return GitLabInterface::issue_view(path, issue_number); }
	);
}

auto GitHostInterface::issue_close(const std::filesystem::path& path, int issue_number)
	-> std::expected<std::string, std::string> {
	return on_host(
		path,
		[&] { return GitHubInterface::issue_close(path, issue_number); },
		[&] { return GitLabInterface::issue_close(path, issue_number); }
	);
}

auto GitHostInterface::request_list(const std::filesystem::path& path) -> std::expected<std::vector<Row>, std::string> {
	return on_host(
		path,
		[&] { return GitHubInterface::request_list(path); },
		[&] { return GitLabInterface::request_list(path); }
	);
}

auto GitHostInterface::request_create(const std::filesystem::path& path) -> std::expected<int, std::string> {
	return on_host(
		path,
		[&] { return GitHubInterface::request_create(path); },
		[&] { return GitLabInterface::request_create(path); }
	);
}

auto GitHostInterface::request_edit(const std::filesystem::path& path, int request_number)
	-> std::expected<int, std::string> {
	return on_host(
		path,
		[&] { return GitHubInterface::request_edit(path, request_number); },
		[&] { return GitLabInterface::request_edit(path, request_number); }
	);
}

auto GitHostInterface::request_view(const std::filesystem::path& path, int request_number)
	-> std::expected<std::string, std::string> {
	return on_host(
		path,
		[&] { return GitHubInterface::request_view(path, request_number); },
		[&] { return GitLabInterface::request_view(path, request_number); }
	);
}

auto GitHostInterface::request_close(const std::filesystem::path& path, int request_number)
	-> std::expected<std::string, std::string> {
	return on_host(
		path,
		[&] { return GitHubInterface::request_close(path, request_number); },
		[&] { return GitLabInterface::request_close(path, request_number); }
	);
}

auto GitHostInterface::request_merge(const std::filesystem::path& path) -> std::expected<std::string, std::string> {
	return on_host(
		path,
		[&] { return GitHubInterface::request_merge(path); },
		[&] { return GitLabInterface::request_merge(path); }
	);
}
