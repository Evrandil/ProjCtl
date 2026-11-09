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

#include "githostinterface.hpp"
#include "gitinterface.hpp"
#include "projectcontent.hpp"
#include "row.hpp"
#include "style.hpp"

auto ProjCtl::request_table(const std::vector<Row>& requests) const -> std::string {
	return render_table(requests, config.tables.request_columns);
}

auto ProjCtl::requests(const ProjectEntry& project) const -> std::expected<std::string, std::string> {
	const ProjectContent& content      = project.second;
	const std::string&    project_name = project.first;

	std::string output = std::format(
		"{}Requests for {}{}{}:\n",
		paint(config.colors.header),
		paint(config.colors.name),
		project_name,
		reset()
	);
	const std::expected list = GitHostInterface::request_list(content.path.string());
	if (!list) {
		return std::unexpected(
			std::format(
				"{}{}Couldn't list requests for {}{}{}: {}{}",
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
		std::format_to(std::back_inserter(output), "{}No requests{}\n", paint(config.colors.header), reset());
		return output;
	}

	std::format_to(std::back_inserter(output), "{}\n", request_table(*list));

	return output;
}

auto ProjCtl::requests(const std::optional<std::string>& name) const -> std::expected<std::string, std::string> {
	const std::expected project = project_find(name);
	if (!project) {
		return std::unexpected(project.error());
	}

	return requests(**project);
}

auto ProjCtl::requests_all() const -> BatchResult {
	std::vector<std::future<std::expected<std::string, std::string>>> tasks;
	tasks.reserve(projects.size());

	const auto is_git = [](const ProjectEntry& project) -> bool { return GitInterface::is_repo(project.second.path); };

	for (const auto& project : projects | std::views::filter(is_git)) {
		tasks.push_back(std::async(std::launch::async, [this, &project] -> std::expected<std::string, std::string> {
			return requests(project);
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

auto ProjCtl::handle(const Cmd::GH::RequestList& cmd) const -> int {
	return cmd.all ? report(requests_all()) : report(requests(cmd.name));
}
