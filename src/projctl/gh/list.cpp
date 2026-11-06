#include "projctl.hpp"

#include "githostinterface.hpp"
#include <future>


BatchResult ProjCtl::repo_list() const
{
	auto list_one = [&](HostOption host) -> std::expected<std::string, std::string>
	{
		std::expected repo_list = GitHostInterface::repo_list(host);
		if (!repo_list)
		{
			return std::unexpected(std::format("{}{} {}repos\n{}{}{}", color_code(config.project_name_color), host == HostOption::GitHub ? "GitHub" : "GitLab", color_code(config.header_color),
						   color_code(config.error_color), repo_list.error(), Style::RESET));
		}
		return std::format("{}{} {}repos\n{}{}{}", color_code(config.project_name_color), host == HostOption::GitHub ? "GitHub" : "GitLab", color_code(config.header_color),
					   color_code(config.project_short_remote_color), *repo_list, Style::RESET);
	};

	std::array<std::future<std::expected<std::string, std::string>>, 2> tasks = std::array<std::future<std::expected<std::string, std::string>>, 2>{
		std::async(std::launch::async, list_one, HostOption::GitHub),
		std::async(std::launch::async, list_one, HostOption::GitLab),
	};

	BatchResult results;
	for (auto& task : tasks)
	{
		std::expected result = task.get();
		if (!result)
		{
			++results.failed;
		}
		std::format_to(std::back_inserter(results.output), "{}\n\n", result ? *result : result.error());
	}
	return results;
}


int ProjCtl::handle(const Cmd::GH::RepoList&) const
{
	return report(repo_list());
}
