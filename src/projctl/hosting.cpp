#include "projctl.hpp"

#include "config.hpp"
#include "githostinterface.hpp"
#include <future>
#include <print>
#include <array>


void ProjCtl::g_status () const {
	auto status_one = [&] (HostOption host) {
		std::string output;
		bool installed = GitHostInterface::installed(host);
		bool authenticated = GitHostInterface::authenticated(host);
		std::format_to(std::back_inserter(output),
				"{}{:<{}}{}{}{}{}\n",
				color_code(config.header_color),
				std::format("{} Cli", "GitLab"),
				config.name_width,
				installed ? color_code(config.tick_color) : color_code(config.error_color),
				installed ? Style::TICK : Style::CROSS,
				installed ? "Installed" : "Not installed",
				Style::RESET
				);
		std::format_to(std::back_inserter(output),
				"{}{:<{}}{}{}{}{}\n",
				color_code(config.header_color),
				"Auth",
				config.name_width,
				authenticated ? color_code(config.tick_color) : color_code(config.error_color),
				authenticated ? Style::TICK : Style::CROSS,
				authenticated ? "Authenticated" : "Not authenticated",
				Style::RESET
				);
		return output;
	};
	std::array<std::future<std::string>, 2> tasks = std::array<std::future<std::string>, 2> {
		std::async(std::launch::async, status_one, HostOption::GitHub),
		std::async(std::launch::async, status_one, HostOption::GitLab),
	};
	for (auto& task : tasks) {
		std::println("{}", task.get());
	}
}


void ProjCtl::repo_list() const {
	auto list_one = [&] (HostOption host) {
		std::string output;
		if (!GitHostInterface::installed(host)) { // TODO is this needed? Could have just authenticated check if is installed as well
			return std::format("{}{}{} is not installed{}", color_code(config.error_color), Style::CROSS, "GitHub", Style::RESET);
		}
		if (!GitHostInterface::authenticated(host)) {
			return std::format("{}{}{} is not authenticated{}", color_code(config.error_color), Style::CROSS, "GitHub", Style::RESET);
		}
		std::expected<std::string, std::string> repo_list = GitHostInterface::repo_list(host);
		std::format_to(std::back_inserter(output),
				"{}{} {}repos\n{}{}{}",
				color_code(config.project_name_color),
				"GitHub", color_code(config.header_color),
				repo_list ? color_code(config.project_short_remote_color) : color_code(config.error_color),
				repo_list ? *repo_list : repo_list.error(),
				Style::RESET
				);
		return output;
	};

	std::array<std::future<std::string>, 2> tasks = std::array<std::future<std::string>, 2> {
		std::async(std::launch::async, list_one, HostOption::GitHub),
		std::async(std::launch::async, list_one, HostOption::GitLab),
	};

	for (auto& task : tasks) {
		std::println("{}", task.get());
	}
}
