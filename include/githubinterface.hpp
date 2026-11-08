#pragma once

#include "clistate.hpp"
#include "issue.hpp"
#include "request.hpp"
#include <expected>
#include <filesystem>
#include <string>
#include <vector>

auto gh_command(std::vector<std::string> args) -> std::vector<std::string>;

namespace GitHubInterface
{
	auto state() -> CliState;
	auto repo_list() -> std::expected<std::string, std::string>;
	auto issue_list(const std::filesystem::path&) -> std::expected<std::vector<Issue>, std::string>;
	auto issue_create(const std::filesystem::path&) -> std::expected<int, std::string>;
	auto issue_edit(const std::filesystem::path&, int) -> std::expected<int, std::string>;
	auto issue_view(const std::filesystem::path&, int) -> std::expected<std::string, std::string>;
	auto issue_close(const std::filesystem::path&, int) -> std::expected<std::string, std::string>;
	auto request_list(const std::filesystem::path&) -> std::expected<std::vector<Request>, std::string>;
	auto request_create(const std::filesystem::path&) -> std::expected<int, std::string>;
	auto request_edit(const std::filesystem::path&, int) -> std::expected<int, std::string>;
	auto request_view(const std::filesystem::path&, int) -> std::expected<std::string, std::string>;
	auto request_close(const std::filesystem::path&, int) -> std::expected<std::string, std::string>;
	auto request_merge(const std::filesystem::path&) -> std::expected<std::string, std::string>;
}; // namespace GitHubInterface
