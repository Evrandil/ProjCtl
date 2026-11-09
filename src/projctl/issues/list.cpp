#include "command.hpp"
#include "field.hpp"
#include "projctl.hpp"

#include <expected>
#include <format>
#include <future>
#include <iterator>
#include <optional>
#include <ranges>
#include <string>
#include <vector>

#include "githostinterface.hpp"
#include "gitinterface.hpp"
#include "projectcontent.hpp"
#include "style.hpp"

auto ProjCtl::issue_table(const std::vector<Field>& issues) const -> std::string {
	return render_table(issues, config.tables.issue_columns);
}

auto ProjCtl::issues(const ProjectEntry& project) const -> std::expected<std::string, std::string> {
	const ProjectContent& content      = project.second;
	const std::string&    project_name = project.first;

	std::string output = std::format(
		"{}Issues for {}{}{}:\n",
		paint(config.colors.header),
		paint(config.colors.name),
		project_name,
		reset()
	);
	const std::expected list = GitHostInterface::issue_list(content.path.string());
	if (!list) {
		return std::unexpected(
			std::format(
				"{}{}Couldn't list issues for {}{}{}: {}{}",
				paint(config.colors.error),
				symbol(Style::Mark::Cross),
				paint(config.colors.name),
				project_name,
				paint(config.colors.error),
				list.error(),
				reset()
			)
		);
	}
	if (list->empty()) {
		std::format_to(std::back_inserter(output), "{}No issues{}\n", paint(config.colors.header), reset());
		return output;
	}

	std::format_to(std::back_inserter(output), "{}\n", issue_table(*list));

	return output;
}

auto ProjCtl::issues(const std::optional<std::string>& name) const -> std::expected<std::string, std::string> {
	const std::expected project = project_find(name);
	if (!project) {
		return std::unexpected(project.error());
	}

	return issues(**project);
}

auto ProjCtl::issues_all() const -> BatchResult {
	std::vector<std::future<std::expected<std::string, std::string>>> tasks;
	tasks.reserve(projects.size());

	const auto is_git = [](const ProjectEntry& project) -> bool { return GitInterface::is_repo(project.second.path); };

	for (const auto& project : projects | std::views::filter(is_git)) {
		tasks.push_back(std::async(std::launch::async, [this, &project] -> std::expected<std::string, std::string> {
			return issues(project);
		}));
	}

	BatchResult results;
	for (decltype(tasks)::value_type& task : tasks) {
		const std::expected result = task.get();
		if (!result) {
			std::format_to(std::back_inserter(results.output), "{}\n", result.error());
			++results.failed;
			continue;
		}
		std::format_to(std::back_inserter(results.output), "{}\n", *result);
	}

	return results;
}

auto ProjCtl::handle(const Cmd::GH::IssueList& cmd) const -> int {
	return cmd.all ? report(issues_all()) : report(issues(cmd.name));
}
