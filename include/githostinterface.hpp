#pragma once

#include <expected>
#include <filesystem>
#include <string>
#include <vector>

#include "clistate.hpp"
#include "githostoption.hpp"
#include "field.hpp"

namespace GitHostInterface {
	[[nodiscard]] auto state(HostOption host) -> CliState;
	[[nodiscard]] auto repo_list(HostOption host) -> std::expected<std::string, std::string>;
	[[nodiscard]] auto issue_list(const std::filesystem::path& path) -> std::expected<std::vector<Field>, std::string>;
	[[nodiscard]] auto issue_create(const std::filesystem::path& path) -> std::expected<int, std::string>;
	[[nodiscard]] auto issue_edit(const std::filesystem::path& path, int issue_number)
		-> std::expected<int, std::string>;
	[[nodiscard]] auto issue_view(const std::filesystem::path& path, int issue_number)
		-> std::expected<std::string, std::string>;
	[[nodiscard]] auto issue_close(const std::filesystem::path& path, int issue_number)
		-> std::expected<std::string, std::string>;
	[[nodiscard]] auto request_list(const std::filesystem::path& path)
		-> std::expected<std::vector<Field>, std::string>;
	[[nodiscard]] auto request_create(const std::filesystem::path& path) -> std::expected<int, std::string>;
	[[nodiscard]] auto request_edit(const std::filesystem::path& path, int request_number)
		-> std::expected<int, std::string>;
	[[nodiscard]] auto request_view(const std::filesystem::path& path, int request_number)
		-> std::expected<std::string, std::string>;
	[[nodiscard]] auto request_close(const std::filesystem::path& path, int request_number)
		-> std::expected<std::string, std::string>;
	[[nodiscard]] auto request_merge(const std::filesystem::path& path) -> std::expected<std::string, std::string>;
}; // namespace GitHostInterface
