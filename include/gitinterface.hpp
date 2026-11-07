#pragma once

#include "githostoption.hpp"
#include <filesystem>
#include <expected>
#include <string>


namespace GitInterface
{
	[[nodiscard]] std::expected<std::string, std::string> add(const std::filesystem::path&);
	[[nodiscard]] bool is_repo(const std::filesystem::path&);
	[[nodiscard]] std::expected<HostOption, std::string> host(const std::filesystem::path&);
	[[nodiscard]] std::expected<std::string, std::string> changes(const std::filesystem::path&);
	[[nodiscard]] std::expected<std::string, std::string> branch(const std::filesystem::path&);
	[[nodiscard]] std::expected<std::string, std::string> branch_list(const std::filesystem::path&);
	[[nodiscard]] std::expected<std::string, std::string> branch_switch(const std::filesystem::path&, std::string_view);
	[[nodiscard]] std::expected<std::string, std::string> remote(const std::filesystem::path&);
	[[nodiscard]] std::expected<std::string, std::string> remote_short(const std::filesystem::path&);
	[[nodiscard]] std::expected<std::string, std::string> commit(const std::filesystem::path&, std::optional<std::string_view>);
	[[nodiscard]] std::expected<std::string, std::string> push(const std::filesystem::path&, bool upstream = false);
	[[nodiscard]] std::expected<std::string, std::string> pull(const std::filesystem::path&);
	[[nodiscard]] std::expected<std::string, std::string> fetch(const std::filesystem::path&);
	[[nodiscard]] std::expected<std::string, std::string> default_merge(const std::filesystem::path&);
	[[nodiscard]] std::expected<std::string, std::string> log(const std::filesystem::path&, bool oneline = false);
	[[nodiscard]] std::expected<int, std::string> diff(const std::filesystem::path&, const std::optional<std::filesystem::path>&);
}; // namespace GitInterface
