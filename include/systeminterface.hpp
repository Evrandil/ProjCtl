#pragma once

#include <expected>
#include <filesystem>
#include <string>

namespace SystemInterface
{
	struct Output
	{
		int         exit_code;
		std::string text;
	};

	[[nodiscard]] auto run(std::span<const std::string> args, const std::filesystem::path& cwd = {}) -> std::expected<SystemInterface::Output, std::string>;
	[[nodiscard]] auto run_interactive(std::span<const std::string> args, const std::filesystem::path& cwd = {}) -> std::expected<int, std::string>;
	[[nodiscard]] auto config_home() -> std::filesystem::path;
	[[nodiscard]] auto data_home() -> std::filesystem::path;
	[[nodiscard]] auto cache_home() -> std::filesystem::path;
}; // namespace SystemInterface
