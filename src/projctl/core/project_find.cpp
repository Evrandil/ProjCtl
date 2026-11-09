#include "projctl.hpp"
#include "style.hpp"
#include <expected>
#include <format>
#include <optional>
#include <string>

auto ProjCtl::project_find(const std::optional<std::string>& name) const
	-> std::expected<ProjectIterator, std::string> {
	std::string actual_name;
	if (name) {
		actual_name = *name;
	} else {
		std::optional<std::string> current = current_project_name();
		if (!current) {
			return std::unexpected(
				std::format(
					"{}{}Current directory isn't a saved project{}",
					paint(config.colors.error),
					Style::CROSS,
					reset()
				)
			);
		}
		actual_name = *current;
	}
	auto project = projects.find(actual_name);
	if (project == projects.end()) {
		return std::unexpected(
			std::format(
				"{}{}Project with name {}{}{} doesn't exist{}",
				paint(config.colors.error),
				Style::CROSS,
				paint(config.colors.name),
				actual_name,
				paint(config.colors.error),
				reset()
			)
		);
	}
	return project;
}
