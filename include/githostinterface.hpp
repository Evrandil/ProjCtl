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
	[[nodiscard]] std::expected<std::vector<Issue>, std::string> issue_list(HostOption, const std::filesystem::path&);
	[[nodiscard]] std::expected<int, std::string> issue_create(HostOption, const std::filesystem::path&);
	[[nodiscard]] std::expected<int, std::string> issue_edit(HostOption, const std::filesystem::path&, int);
	[[nodiscard]] std::expected<std::string, std::string> issue_close(HostOption, const std::filesystem::path&, int);
	[[nodiscard]] std::expected<std::vector<Request>, std::string> request_list(HostOption, const std::filesystem::path&);
	[[nodiscard]] std::expected<int, std::string> request_create(HostOption, const std::filesystem::path&);
	[[nodiscard]] std::expected<int, std::string> request_edit(HostOption, const std::filesystem::path&, int);
	[[nodiscard]] std::expected<std::string, std::string> request_close(HostOption, const std::filesystem::path&, int);
	[[nodiscard]] std::expected<std::string, std::string> request_merge(HostOption, const std::filesystem::path&);
}; // namespace GitHostInterface
