#pragma once

#include "issue.hpp"
#include "request.hpp"

#include <vector>
#include <expected>
#include <string>

namespace GitHubInterface {
	bool installed();
	bool authenticated();
	std::expected<std::string, std::string>repo_list();
	std::expected<std::vector<Issue>, std::string>issue_list(std::string_view);
	std::expected<void, std::string>issue_create(std::string_view);
	std::expected<std::vector<Request>, std::string> request_list(std::string_view);
	std::expected<void, std::string> request_create(std::string_view);
	std::expected<std::string, std::string> request_merge(std::string_view);
};
