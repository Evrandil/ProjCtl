#pragma once

#include "issue.hpp"
#include "request.hpp"
#include "clistate.hpp"
#include <filesystem>
#include <expected>
#include <string>


namespace GitLabInterface
{
	CliState state();
	std::expected<std::string, std::string> repo_list();
	std::expected<std::vector<Issue>, std::string> issue_list(const std::filesystem::path&);
	std::expected<int, std::string> issue_create(const std::filesystem::path&);
	std::expected<int, std::string> issue_edit(const std::filesystem::path&, int);
	std::expected<std::string, std::string> issue_close(const std::filesystem::path&, int);
	std::expected<std::vector<Request>, std::string> request_list(const std::filesystem::path&);
	std::expected<int, std::string> request_create(const std::filesystem::path&);
	std::expected<int, std::string> request_edit(const std::filesystem::path&, int);
	std::expected<std::string, std::string> request_close(const std::filesystem::path&, int);
	std::expected<std::string, std::string> request_merge(const std::filesystem::path&);
}; // namespace GitLabInterface
