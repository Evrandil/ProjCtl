#include "projctl.hpp"

#include "style.hpp"

#include <expected>
#include <format>
#include <optional>
#include <string>

auto ProjCtl::project_find(const std::optional<std::string>& name) const
	-> std::expected<ProjectIterator, std::string> {
	const auto actual_name = name.or_else([this] { return current_project_name(); });
	if (!actual_name) {
		return std::unexpected(
			std::format(
				"{}{}Current directory isn't a saved project{}",
				paint(config.colors.error),
				Style::CROSS,
				reset()
			)
		);
	}

	if (const auto project = projects.find(*actual_name); project != projects.end()) {
		return project;
	}

	return std::unexpected(
		std::format(
			"{}{}Project with name {}{}{} doesn't exist{}",
			paint(config.colors.error),
			Style::CROSS,
			paint(config.colors.name),
			*actual_name,
			paint(config.colors.error),
			reset()
		)
	);
}
