#include "clistate.hpp"
#include "command.hpp"
#include "githostoption.hpp"
#include "projctl.hpp"

#include <array>
#include <format>
#include <future>
#include <iterator>
#include <string>

#include "githostinterface.hpp"
#include "style.hpp"

auto ProjCtl::g_status() const -> BatchResult {
	auto status_one = [&](HostOption host) -> std::string {
		std::string output;
		CliState    state = GitHostInterface::state(host);
		std::format_to(
			std::back_inserter(output),
			"{}{:<{}}{}{}{}{}\n",
			paint(config.header_color),
			std::format("{} Cli", host == HostOption::GitHub ? "GitHub" : "GitLab"),
			config.name_width,
			state != CliState::NotInstalled ? paint(config.tick_color) : paint(config.error_color),
			state != CliState::NotInstalled ? Style::TICK : Style::CROSS,
			state != CliState::NotInstalled ? "Installed" : "Not installed",
			reset()
		);
		std::format_to(
			std::back_inserter(output),
			"{}{:<{}}{}{}{}{}\n",
			paint(config.header_color),
			"Auth",
			config.name_width,
			state == CliState::Ready ? paint(config.tick_color) : paint(config.error_color),
			state == CliState::Ready ? Style::TICK : Style::CROSS,
			state == CliState::Ready ? "Authenticated" : "Not authenticated",
			reset()
		);
		return output;
	};
	std::array<std::future<std::string>, 2> tasks = std::array<std::future<std::string>, 2>{
		std::async(std::launch::async, status_one, HostOption::GitHub),
		std::async(std::launch::async, status_one, HostOption::GitLab),
	};

	BatchResult results;
	for (auto& task : tasks) {
		std::string result = task.get();
		std::format_to(std::back_inserter(results.output), "{}\n", result);
	}
	return results;
}

auto ProjCtl::handle(const Cmd::GH::Status&) const -> int {
	return report(g_status());
}
