#pragma once

#include <expected>
#include <filesystem>
#include <string>

#include "clistate.hpp"
#include "field.hpp"

namespace GitLabInterface {
	auto state() -> CliState;
	auto repo_list() -> std::expected<std::string, std::string>;
	auto issue_list(const std::filesystem::path& path) -> std::expected<std::vector<Field>, std::string>;
	auto issue_create(const std::filesystem::path& path) -> std::expected<int, std::string>;
	auto issue_edit(const std::filesystem::path& path, int issue_number) -> std::expected<int, std::string>;
	auto issue_view(const std::filesystem::path& path, int issue_number) -> std::expected<std::string, std::string>;
	auto issue_close(const std::filesystem::path& path, int issue_number) -> std::expected<std::string, std::string>;
	auto request_list(const std::filesystem::path& path) -> std::expected<std::vector<Field>, std::string>;
	auto request_create(const std::filesystem::path& path) -> std::expected<int, std::string>;
	auto request_edit(const std::filesystem::path& path, int request_number) -> std::expected<int, std::string>;
	auto request_view(const std::filesystem::path& path, int request_number) -> std::expected<std::string, std::string>;
	auto request_close(const std::filesystem::path& path, int request_number)
		-> std::expected<std::string, std::string>;
	auto request_merge(const std::filesystem::path& path) -> std::expected<std::string, std::string>;
}; // namespace GitLabInterface
