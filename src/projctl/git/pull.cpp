#include "command.hpp"
#include "projctl.hpp"

#include <expected>
#include <format>
#include <future>
#include <iterator>
#include <optional>
#include <ranges>
#include <string>
#include <vector>

#include "gitinterface.hpp"
#include "style.hpp"

auto ProjCtl::pull(const ProjectEntry& project) const -> std::expected<std::string, std::string> {
	std::string output;
	std::format_to(
		std::back_inserter(output),
		"{}Pulling: {}{}{}\n",
		paint(config.header_color),
		paint(config.project_name_color),
		project.first,
		reset()
	);
	std::expected<std::string, std::string> result = GitInterface::pull(project.second.path);

	if (!result) {
		std::format_to(
			std::back_inserter(output),
			"{}{}Couldn't pull {}{}{}: {}{}",
			paint(config.error_color),
			Style::CROSS,
			paint(config.project_name_color),
			project.first,
			paint(config.error_color),
			result.error(),
			reset()
		);
		return std::unexpected(output);
	}

	std::format_to(std::back_inserter(output), "{}{}{}{}", paint(config.tick_color), Style::TICK, *result, reset());
	return output;
}

auto ProjCtl::pull(const std::optional<std::string>& name) const -> std::expected<std::string, std::string> {
	std::expected project = project_find(name);
	if (!project) {
		return std::unexpected(project.error());
	}

	return pull(**project);
}

auto ProjCtl::pull_all() const -> BatchResult {
	std::vector<std::future<std::expected<std::string, std::string>>> tasks;
	tasks.reserve(projects.size());

	const auto is_git = [](const ProjectEntry& project) -> bool { return GitInterface::is_repo(project.second.path); };

	for (const ProjectEntry& project : projects | std::views::filter(is_git)) {
		tasks.push_back(std::async(std::launch::async, [this, &project] -> std::expected<std::string, std::string> {
			return pull(project);
		}));
	}

	BatchResult results;
	for (std::future<std::expected<std::string, std::string>>& task : tasks) {
		std::expected pulled = task.get();
		if (!pulled) {
			std::format_to(std::back_inserter(results.output), "{}\n", pulled.error());
			++results.failed;
			continue;
		}
		std::format_to(std::back_inserter(results.output), "{}\n\n", *pulled);
	}

	return results;
}

auto ProjCtl::handle(const Cmd::Git::Pull& cmd) const -> int {
	return cmd.all ? report(pull_all()) : report(pull(cmd.name));
}
