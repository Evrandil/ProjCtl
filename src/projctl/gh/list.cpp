#include "projctl.hpp"

#include "githostinterface.hpp"
#include <future>


std::expected<std::string, std::string> ProjCtl::repo_list() const
{
	auto list_one = [&](HostOption host)
	{
		std::string output;
		std::expected repo_list = GitHostInterface::repo_list(host);
		if (!repo_list)
		{
			std::format_to(std::back_inserter(output), "{}{} {}repos\n{}{}{}", color_code(config.project_name_color), host == HostOption::GitHub ? "GitHub" : "GitLab", color_code(config.header_color),
						   color_code(config.error_color), repo_list.error(), Style::RESET);
			return output;
		}
		std::format_to(std::back_inserter(output), "{}{} {}repos\n{}{}{}", color_code(config.project_name_color), host == HostOption::GitHub ? "GitHub" : "GitLab", color_code(config.header_color),
					   color_code(config.project_short_remote_color), *repo_list, Style::RESET);
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


int ProjCtl::handle(const Cmd::GH::RepoList&) const
{
	return report(repo_list());
}
