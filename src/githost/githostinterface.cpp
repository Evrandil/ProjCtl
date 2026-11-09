#include "githostinterface.hpp"

#include <expected>
#include <string>
#include <utility>

#include "clistate.hpp"
#include "githostoption.hpp"
#include "githubinterface.hpp"
#include "gitlabinterface.hpp"

auto GitHostInterface::state(const HostOption host) -> CliState {
	switch (host) {
		case HostOption::GitHub:  return GitHubInterface::state();
		case HostOption::GitLab:  return GitLabInterface::state();
		case HostOption::Unknown: return CliState::NotInstalled;
	}
	std::unreachable();
}

auto GitHostInterface::repo_list(const HostOption host) -> std::expected<std::string, std::string> {
	switch (host) {
		case HostOption::GitHub:  return GitHubInterface::repo_list();
		case HostOption::GitLab:  return GitLabInterface::repo_list();
		case HostOption::Unknown: return std::unexpected("Unknown host");
	}
	std::unreachable();
}
