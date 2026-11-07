#pragma once

#include "issue.hpp"
#include "request.hpp"
#include "clistate.hpp"

#include <vector>
#include <expected>
#include <string>

std::vector<std::string> gh_command(std::vector<std::string> args);


namespace GitHubInterface
{
	CliState state();
	std::expected<std::string, std::string> repo_list();
	std::expected<std::vector<Issue>, std::string> issue_list(std::string_view);
	std::expected<int, std::string> issue_create(std::string_view);
	std::expected<int, std::string> issue_edit(std::string_view, int);
	std::expected<std::string, std::string> issue_close(std::string_view, int);
	std::expected<std::vector<Request>, std::string> request_list(std::string_view);
	std::expected<int, std::string> request_create(std::string_view);
	std::expected<int, std::string> request_edit(std::string_view, int);
	std::expected<std::string, std::string> request_close(std::string_view, int);
	std::expected<std::string, std::string> request_merge(std::string_view);
}; // namespace GitHubInterface
