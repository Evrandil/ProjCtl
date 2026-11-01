#include "githostinterface.hpp"

#include "githubinterface.hpp"
#include "gitlabinterface.hpp"


bool GitHostInterface::installed(HostOption host) {
	switch (host) {
		case HostOption::GitHub:		return GitHubInterface::installed();
		case HostOption::GitLab:	 	return GitLabInterface::installed();
		default:						return false;
	}
}


bool GitHostInterface::authenticated(HostOption host) {
	switch (host) {
		case HostOption::GitHub:		return GitHubInterface::authenticated();
		case HostOption::GitLab:	 	return GitLabInterface::authenticated();
		default:						return false;
	}
}


std::expected<std::string, std::string> GitHostInterface::repo_list(HostOption host) {
	switch (host) {
		case HostOption::GitHub:		return GitHubInterface::repo_list();
		case HostOption::GitLab:	 	return GitLabInterface::repo_list();
		default:						return std::unexpected("Unknown host");
	}
}


std::expected<std::vector<Issue>, std::string> GitHostInterface::issue_list(HostOption host, std::string_view remote_short) {
	switch (host) {
		case HostOption::GitHub:		return GitHubInterface::issue_list(remote_short);
		case HostOption::GitLab:	 	return GitLabInterface::issue_list(remote_short);
		default:						return std::unexpected("Unknown host");
	}
}


std::expected<void, std::string> GitHostInterface::issue_create(HostOption host, std::string_view remote_short) {
	switch (host) {
		case HostOption::GitHub:		return GitHubInterface::issue_create(remote_short);
		case HostOption::GitLab:	 	return GitLabInterface::issue_create(remote_short);
		default:						return std::unexpected("Unknown host");
	}
}


std::expected<std::vector<Request>, std::string> GitHostInterface::request_list(HostOption host, std::string_view remote_short) {
	switch (host) {
		case HostOption::GitHub:		return GitHubInterface::request_list(remote_short);
		case HostOption::GitLab:	 	return GitLabInterface::request_list(remote_short);
		default:						return std::unexpected("Unknown host");
	}
}


std::expected<void, std::string> GitHostInterface::request_create(HostOption host, std::string_view path) {
	switch (host) {
		case HostOption::GitHub:		return GitHubInterface::request_create(path);
		case HostOption::GitLab:	 	return GitLabInterface::request_create(path);
		default:						return std::unexpected("Unknown host");
	}
}


std::expected<std::string, std::string> GitHostInterface::request_merge(HostOption host, std::string_view path) {
	switch (host) {
		case HostOption::GitHub:		return GitHubInterface::request_merge(path);
		case HostOption::GitLab:	 	return GitLabInterface::request_merge(path);
		default:						return std::unexpected("Unknown host");
	}
}
