#include "projctl.hpp"

#include "config.hpp"
#include "githubinterface.hpp"
#include "gitlabinterface.hpp"
#include <future>
#include <print>


void ProjCtl::g_status () const {
	auto status_gh = [&] () {
		std::string output;
		bool installed = GitHubInterface::installed();
		bool authenticated = GitHubInterface::authenticated();
		std::format_to(std::back_inserter(output), "{}{:<{}}{}{}{}{}\n", color_code(config.header_color), std::format("{} Cli", "GitLab"), config.name_width, installed ? color_code(config.tick_color) : color_code(config.error_color), installed ? Style::TICK : Style::CROSS, installed ? "Installed" : "Not installed", Style::RESET);
		std::format_to(std::back_inserter(output), "{}{:<{}}{}{}{}{}\n", color_code(config.header_color), "Auth", config.name_width, authenticated ? color_code(config.tick_color) : color_code(config.error_color), authenticated ? Style::TICK : Style::CROSS, authenticated ? "Authenticated" : "Not authenticated", Style::RESET);
		return output;
	};
	auto status_glab = [&] () {
		std::string output;
		bool installed = GitLabInterface::installed();
		bool authenticated = GitLabInterface::authenticated();
		std::format_to(std::back_inserter(output), "{}{:<{}}{}{}{}{}\n", color_code(config.header_color), std::format("{} Cli", "GitLab"), config.name_width, installed ? color_code(config.tick_color) : color_code(config.error_color), installed ? Style::TICK : Style::CROSS, installed ? "Installed" : "Not installed", Style::RESET);
		std::format_to(std::back_inserter(output), "{}{:<{}}{}{}{}{}\n", color_code(config.header_color), "Auth", config.name_width, authenticated ? color_code(config.tick_color) : color_code(config.error_color), authenticated ? Style::TICK : Style::CROSS, authenticated ? "Authenticated" : "Not authenticated", Style::RESET);
		return output;
	};
	std::future<std::string> gh_task = std::async(std::launch::async, status_gh);
	std::future<std::string> glab_task = std::async(std::launch::async, status_glab);
	std::println("{}", gh_task.get());
	std::println("{}", glab_task.get());
}


void ProjCtl::repo_list() const {
	auto list_gh = [&] () {
		std::string output;
		if (!GitHubInterface::installed()) { // TODO is this needed? Could have just authenticated check if is installed as well
			return std::format("{}{}{} is not installed{}", color_code(config.error_color), Style::CROSS, "GitHub", Style::RESET);
		}
		if (!GitHubInterface::authenticated()) {
			return std::format("{}{}{} is not authenticated{}", color_code(config.error_color), Style::CROSS, "GitHub", Style::RESET);
		}
		std::expected<std::string, std::string> repo_list = GitHubInterface::repo_list();
		std::format_to(std::back_inserter(output), "{}{} {}repos\n{}{}{}", color_code(config.project_name_color), "GitHub", color_code(config.header_color), repo_list ? color_code(config.project_short_remote_color) : color_code(config.error_color), repo_list ? *repo_list : repo_list.error(), Style::RESET);
		return output;
	};
	auto list_glab = [&] () {
		std::string output;
		if (!GitLabInterface::installed()) { // TODO is this needed? Could have just authenticated check if is installed as well
			return std::format("{}{}{} is not installed{}", color_code(config.error_color), Style::CROSS, "GitLab", Style::RESET);
		}
		if (!GitLabInterface::authenticated()) {
			return std::format("{}{}{} is not authenticated{}", color_code(config.error_color), Style::CROSS, "GitLab", Style::RESET);
		}
		std::expected<std::string, std::string> repo_list = GitLabInterface::repo_list();
		std::format_to(std::back_inserter(output), "{}{} {}repos\n{}{}{}", color_code(config.project_name_color), "GitLab", color_code(config.header_color), repo_list ? color_code(config.project_short_remote_color) : color_code(config.error_color), repo_list ? *repo_list : repo_list.error(), Style::RESET);
		return output;
	};

	std::future<std::string> gh_task = std::async(std::launch::async, list_gh);
	std::future<std::string> glab_task = std::async(std::launch::async, list_glab);

	std::println("{}\n", gh_task.get());
	std::println("{}", glab_task.get());
}
