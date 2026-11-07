#pragma once

#include "issue.hpp"
#include "request.hpp"
#include "githostoption.hpp"
#include "clistate.hpp"
#include <filesystem>
#include <vector>
#include <expected>
#include <string>






namespace GitHostInterface
{
	[[nodiscard]] CliState state(HostOption);
	[[nodiscard]] std::expected<std::string, std::string> repo_list(HostOption);
	[[nodiscard]] std::expected<std::vector<Issue>, std::string> issue_list(const std::filesystem::path&);
	[[nodiscard]] std::expected<int, std::string> issue_create(const std::filesystem::path&);
	[[nodiscard]] std::expected<int, std::string> issue_edit(const std::filesystem::path&, int);
	[[nodiscard]] std::expected<std::string, std::string> issue_view(const std::filesystem::path&, int);
	[[nodiscard]] std::expected<std::string, std::string> issue_close(const std::filesystem::path&, int);
	[[nodiscard]] std::expected<std::vector<Request>, std::string> request_list(const std::filesystem::path&);
	[[nodiscard]] std::expected<int, std::string> request_create(const std::filesystem::path&);
	[[nodiscard]] std::expected<int, std::string> request_edit(const std::filesystem::path&, int);
	[[nodiscard]] std::expected<std::string, std::string> request_view(const std::filesystem::path&, int);
	[[nodiscard]] std::expected<std::string, std::string> request_close(const std::filesystem::path&, int);
	[[nodiscard]] std::expected<std::string, std::string> request_merge(const std::filesystem::path&);
}; // namespace GitHostInterface
