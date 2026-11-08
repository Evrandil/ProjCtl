#include "projctl.hpp"

#include <future>

#include "githostinterface.hpp"

auto ProjCtl::repo_list() const -> BatchResult {
	auto list_one = [&](HostOption host) -> std::expected<std::string, std::string> {
		std::expected repo_list = GitHostInterface::repo_list(host);
		if (!repo_list) {
			return std::unexpected(
				std::format(
					"{}{} {}repos\n{}{}{}",
					paint(config.project_name_color),
					host == HostOption::GitHub ? "GitHub" : "GitLab",
					paint(config.header_color),
					paint(config.error_color),
					repo_list.error(),
					reset()
				)
			);
		}
		return std::format(
			"{}{} {}repos\n{}{}{}",
			paint(config.project_name_color),
			host == HostOption::GitHub ? "GitHub" : "GitLab",
			paint(config.header_color),
			paint(config.project_short_remote_color),
			*repo_list,
			reset()
		);
	};

	std::array<std::future<std::expected<std::string, std::string>>, 2> tasks =
		std::array<std::future<std::expected<std::string, std::string>>, 2>{
			std::async(std::launch::async, list_one, HostOption::GitHub),
			std::async(std::launch::async, list_one, HostOption::GitLab),
	};

	BatchResult results;
	for (auto& task : tasks) {
		std::expected result = task.get();
		if (!result) {
			++results.failed;
		}
		std::format_to(std::back_inserter(results.output), "{}\n\n", result ? *result : result.error());
	}
	return results;
}

auto ProjCtl::handle(const Cmd::GH::RepoList&) const -> int {
	return report(repo_list());
}
