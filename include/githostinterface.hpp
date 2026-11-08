#pragma once

#include "clistate.hpp"
#include "githostoption.hpp"
#include "issue.hpp"
#include "request.hpp"
#include <expected>
#include <filesystem>
#include <string>
#include <vector>

namespace GitHostInterface
{
	[[nodiscard]] auto state(HostOption) -> CliState;
	[[nodiscard]] auto repo_list(HostOption) -> std::expected<std::string, std::string>;
	[[nodiscard]] auto issue_list(const std::filesystem::path&) -> std::expected<std::vector<Issue>, std::string>;
	[[nodiscard]] auto issue_create(const std::filesystem::path&) -> std::expected<int, std::string>;
	[[nodiscard]] auto issue_edit(const std::filesystem::path&, int) -> std::expected<int, std::string>;
	[[nodiscard]] auto issue_view(const std::filesystem::path&, int) -> std::expected<std::string, std::string>;
	[[nodiscard]] auto issue_close(const std::filesystem::path&, int) -> std::expected<std::string, std::string>;
	[[nodiscard]] auto request_list(const std::filesystem::path&) -> std::expected<std::vector<Request>, std::string>;
	[[nodiscard]] auto request_create(const std::filesystem::path&) -> std::expected<int, std::string>;
	[[nodiscard]] auto request_edit(const std::filesystem::path&, int) -> std::expected<int, std::string>;
	[[nodiscard]] auto request_view(const std::filesystem::path&, int) -> std::expected<std::string, std::string>;
	[[nodiscard]] auto request_close(const std::filesystem::path&, int) -> std::expected<std::string, std::string>;
	[[nodiscard]] auto request_merge(const std::filesystem::path&) -> std::expected<std::string, std::string>;
}; // namespace GitHostInterface
