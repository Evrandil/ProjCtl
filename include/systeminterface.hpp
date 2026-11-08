#pragma once

#include <expected>
#include <string>
#include <filesystem>


namespace SystemInterface
{
	struct Output
	{
		int exit_code;
		std::string text;
	};

	[[nodiscard]] std::expected<SystemInterface::Output, std::string> run(std::span<const std::string> args,
																		  const std::filesystem::path& cwd = {});
	[[nodiscard]] std::expected<int, std::string> run_interactive(std::span<const std::string> args,
																  const std::filesystem::path& cwd = {});
	[[nodiscard]] std::filesystem::path config_home();
	[[nodiscard]] std::filesystem::path data_home();
	[[nodiscard]] std::filesystem::path cache_home();
}; // namespace SystemInterface
