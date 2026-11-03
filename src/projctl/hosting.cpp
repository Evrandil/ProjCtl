#include "projctl.hpp"

#include "config.hpp"
#include "githostinterface.hpp"
#include <future>
#include <array>


std::expected<std::string, std::string> ProjCtl::g_status() const
{
	auto status_one = [&](HostOption host)
	{
		std::string output;
		CliState state = GitHostInterface::state(host);
		std::format_to(std::back_inserter(output), "{}{:<{}}{}{}{}{}\n", color_code(config.header_color), std::format("{} Cli", host == HostOption::GitHub ? "GitHub" : "GitLab"), config.name_width, state != CliState::NotInstalled ? color_code(config.tick_color) : color_code(config.error_color),
					   state != CliState::NotInstalled ? Style::TICK : Style::CROSS, state != CliState::NotInstalled ? "Installed" : "Not installed", Style::RESET);
		std::format_to(std::back_inserter(output), "{}{:<{}}{}{}{}{}\n", color_code(config.header_color), "Auth", config.name_width, state == CliState::Ready ? color_code(config.tick_color) : color_code(config.error_color), state == CliState::Ready ? Style::TICK : Style::CROSS,
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


std::expected<std::string, std::string> ProjCtl::repo_list() const
{
	auto list_one = [&](HostOption host)
	{
		std::string output;
		std::expected repo_list = GitHostInterface::repo_list(host);
		if (!repo_list)
		{
			std::format_to(std::back_inserter(output), "{}{} {}repos\n{}{}{}", color_code(config.project_name_color), host == HostOption::GitHub ? "GitHub" : "GitLab", color_code(config.header_color), color_code(config.error_color), repo_list.error(), Style::RESET);
			return output;
		}
		std::format_to(std::back_inserter(output), "{}{} {}repos\n{}{}{}", color_code(config.project_name_color), host == HostOption::GitHub ? "GitHub" : "GitLab", color_code(config.header_color), color_code(config.project_short_remote_color), *repo_list, Style::RESET);
		return output;
	};

	std::array<std::future<std::string>, 2> tasks = std::array<std::future<std::string>, 2>{
		std::async(std::launch::async, list_one, HostOption::GitHub),
		std::async(std::launch::async, list_one, HostOption::GitLab),
	};

	std::string output;
	for (auto& task : tasks)
	{
		std::format_to(std::back_inserter(output), "{}\n\n", task.get());
	}
	return output;
}
