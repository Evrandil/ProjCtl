#pragma once

#include <filesystem>
#include <optional>
#include <string>

struct ProjectContent {
	std::filesystem::path      path;
	std::optional<std::string> build_command = std::nullopt;
	std::optional<std::string> run_command   = std::nullopt;
};
