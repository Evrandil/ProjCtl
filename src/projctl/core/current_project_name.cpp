#include "projctl.hpp"
#include <algorithm>
#include <cstddef>
#include <filesystem>
#include <iterator>
#include <optional>
#include <string>
#include <system_error>

auto ProjCtl::current_project_name() const -> std::optional<std::string> {
	std::error_code             error;
	const std::filesystem::path cwd = std::filesystem::weakly_canonical(std::filesystem::current_path(error), error);
	if (error) {
		return std::nullopt;
	}
	const ProjectEntry* best       = nullptr;
	std::ptrdiff_t      best_depth = 0;
	for (const ProjectEntry& project : projects) {
		const std::filesystem::path project_path = std::filesystem::weakly_canonical(project.second.path, error);
		if (error) {
			continue;
		}

		const auto [project_end, cwd_rest] = std::ranges::mismatch(project_path, cwd);
		if (project_end != project_path.end()) {
			continue;
		}

		const std::ptrdiff_t depth = std::ranges::distance(project_path);
		if (depth > best_depth) {
			best       = &project;
			best_depth = depth;
		}
	}

	if (best == nullptr) {
		return std::nullopt;
	}
	return best->first;
}
