#include "projctl.hpp"

#include <expected>
#include <format>
#include <functional>
#include <future>
#include <iterator>
#include <ranges>
#include <string>
#include <vector>

#include "gitinterface.hpp"
#include "projectcontent.hpp"

auto ProjCtl::list_gits(bool names_only) const -> std::expected<std::string, std::string> {
	const auto list_one_git = [&](const std::string& name, const ProjectContent& content) -> std::string {
		const std::expected branch       = GitInterface::branch(content.path);
		const std::expected remote_short = GitInterface::remote_short(content.path);
		return std::format(
			"{}{:<{}}{}{:<{}}{}{:<{}}{}{:<{}}\n",
			paint(config.colors.name),
			truncate(name, config.widths.name),
			config.widths.name,
			paint(config.colors.path),
			truncate(content.path.string(), config.widths.path),
			config.widths.path,
			paint(config.colors.branch),
			truncate(branch.value_or("-"), config.widths.branch),
			config.widths.branch,
			paint(config.colors.short_remote),
			truncate(remote_short.value_or("-"), config.widths.remote),
			config.widths.remote
		);
	};

	std::vector<std::future<std::string>> tasks;
	tasks.reserve(projects.size());

	const auto is_git = [](const ProjectEntry& project) -> bool { return GitInterface::is_repo(project.second.path); };

	std::string output;

	if (names_only) {
		for (const ProjectEntry& project : projects | std::views::filter(is_git)) {
			std::format_to(std::back_inserter(output), "{}\n", project.first);
		}
		return output;
	}

	for (const ProjectEntry& project : projects | std::views::filter(is_git)) {
		tasks.push_back(
			std::async(std::launch::async, list_one_git, std::cref(project.first), std::cref(project.second))
		);
	}

	if (tasks.empty()) [[unlikely]] {
		return std::unexpected(
			std::format("{}There are no git projects saved{}\n", paint(config.colors.error), reset())
		);
	}

	std::format_to(
		std::back_inserter(output),
		"{}{:<{}}{:<{}}{:<{}}{:<{}}{}\n",
		paint(config.colors.header),
		"Name",
		config.widths.name,
		"Path",
		config.widths.path,
		"Branch",
		config.widths.branch,
		"Repo",
		config.widths.remote,
		reset()
	);

	for (std::future<std::string>& task : tasks) {
		std::format_to(std::back_inserter(output), "{}", task.get());
	}
	return output;
}
