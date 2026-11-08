#include "githostinterface.hpp"

#include <expected>
#include <string>

#include "clistate.hpp"
#include "githostoption.hpp"
#include "githubinterface.hpp"
#include "gitlabinterface.hpp"

auto GitHostInterface::state(HostOption host) -> CliState {
	switch (host) {
		case HostOption::GitHub: return GitHubInterface::state();
		case HostOption::GitLab: return GitLabInterface::state();
		default:                 return CliState::NotInstalled;
	}
}

auto GitHostInterface::repo_list(HostOption host) -> std::expected<std::string, std::string> {
	switch (host) {
		case HostOption::GitHub: return GitHubInterface::repo_list();
		case HostOption::GitLab: return GitLabInterface::repo_list();
		default:                 return std::unexpected("Unknown host");
	}
}
