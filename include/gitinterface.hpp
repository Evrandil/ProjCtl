#pragma once

#include <expected>
#include <filesystem>
#include <string>

#include "githostoption.hpp"

namespace GitInterface {
	[[nodiscard]] auto add(const std::filesystem::path& path) -> std::expected<std::string, std::string>;
	[[nodiscard]] auto is_repo(const std::filesystem::path& path) -> bool;
	[[nodiscard]] auto host(const std::filesystem::path& path) -> std::expected<HostOption, std::string>;
	[[nodiscard]] auto changes(const std::filesystem::path& path) -> std::expected<std::string, std::string>;
	[[nodiscard]] auto branch(const std::filesystem::path& path) -> std::expected<std::string, std::string>;
	[[nodiscard]] auto branch_list(const std::filesystem::path& path) -> std::expected<std::string, std::string>;
	[[nodiscard]] auto branch_switch(const std::filesystem::path& path, std::string_view branch)
		-> std::expected<std::string, std::string>;
	[[nodiscard]] auto remote(const std::filesystem::path& path) -> std::expected<std::string, std::string>;
	[[nodiscard]] auto remote_short(const std::filesystem::path& path) -> std::expected<std::string, std::string>;
	[[nodiscard]] auto commit(const std::filesystem::path& path, std::optional<std::string_view> message)
		-> std::expected<std::string, std::string>;
	[[nodiscard]] auto push(const std::filesystem::path& path, bool upstream = false)
		-> std::expected<std::string, std::string>;
	[[nodiscard]] auto pull(const std::filesystem::path& path) -> std::expected<std::string, std::string>;
	[[nodiscard]] auto fetch(const std::filesystem::path& path) -> std::expected<std::string, std::string>;
	[[nodiscard]] auto default_merge(const std::filesystem::path& path) -> std::expected<std::string, std::string>;
	[[nodiscard]] auto log(const std::filesystem::path& path, bool oneline = false)
		-> std::expected<std::string, std::string>;
	[[nodiscard]] auto diff(const std::filesystem::path& path, const std::optional<std::filesystem::path>& file_path)
		-> std::expected<int, std::string>;
} // namespace GitInterface
