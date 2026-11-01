#pragma once

#include "issue.hpp"
#include "request.hpp"
#include "gitinterface.hpp"

#include <vector>
#include <expected>
#include <string>

namespace GitHostInterface {
	bool installed(HostOption);
	bool authenticated(HostOption);
	std::expected<std::string, std::string>repo_list(HostOption);
	std::expected<std::vector<Issue>, std::string>issue_list(HostOption, std::string_view);
	std::expected<void, std::string>issue_create(HostOption, std::string_view);
	std::expected<std::vector<Request>, std::string> request_list(HostOption, std::string_view);
	std::expected<void, std::string> request_create(HostOption, std::string_view);
	std::expected<std::string, std::string> request_merge(HostOption, std::string_view);
};
