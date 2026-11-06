#include "projctl.hpp"

#include "githostinterface.hpp"
#include <future>


std::expected<std::string, std::string> ProjCtl::g_status() const
{
	auto status_one = [&](HostOption host)
	{
		std::string output;
		CliState state = GitHostInterface::state(host);
		std::format_to(std::back_inserter(output), "{}{:<{}}{}{}{}{}\n", color_code(config.header_color), std::format("{} Cli", host == HostOption::GitHub ? "GitHub" : "GitLab"), config.name_width,
					   state != CliState::NotInstalled ? color_code(config.tick_color) : color_code(config.error_color), state != CliState::NotInstalled ? Style::TICK : Style::CROSS,
					   state != CliState::NotInstalled ? "Installed" : "Not installed", Style::RESET);
		std::format_to(std::back_inserter(output), "{}{:<{}}{}{}{}{}\n", color_code(config.header_color), "Auth", config.name_width,
					   state == CliState::Ready ? color_code(config.tick_color) : color_code(config.error_color), state == CliState::Ready ? Style::TICK : Style::CROSS,
					   state == CliState::Ready ? "Authenticated" : "Not authenticated", Style::RESET);
		return output;
	};
	std::array<std::future<std::string>, 2> tasks = std::array<std::future<std::string>, 2>{
		std::async(std::launch::async, status_one, HostOption::GitHub),
		std::async(std::launch::async, status_one, HostOption::GitLab),
	};

	std::string output;
	for (auto& task : tasks)
	{
		std::format_to(std::back_inserter(output), "{}\n", task.get());
	}
	return output;
}


int ProjCtl::handle(const Cmd::GH::Status&) const
{
	return report(g_status());
}
