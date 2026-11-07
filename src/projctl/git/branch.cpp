#include "projctl.hpp"

#include "gitinterface.hpp"


std::expected<std::string, std::string> ProjCtl::branch(const std::optional<std::string>& name, const std::optional<std::string>& branch_name) const
{
	std::expected project = project_find(name);
	if (!project)
	{
		return std::unexpected(project.error());
	}
	const ProjectContent& content = (*project)->second;

	if (!branch_name)
	{
		std::expected result = GitInterface::branch(content.path);
		if (!result)
		{
			return std::unexpected(std::format("{}{}{}", paint(config.error_color), result.error(), reset()));
		}

		return std::format("{}{}{}", paint(config.project_branch_color), *result, reset());
	}

	std::expected result = GitInterface::branch_switch(content.path, *branch_name);
	if (!result)
	{
		return std::unexpected(std::format("{}Couldn't switch branches for {}{}{}: {}{}", paint(config.error_color), paint(config.project_name_color), (*project)->first, paint(config.error_color), result.error(), reset()));
	}
	return std::format("{}{}{}{}", paint(config.tick_color), Style::TICK, *result, reset());
}


int ProjCtl::handle(const Cmd::Git::Branch& cmd) const
{
	return report(branch(cmd.name, cmd.branch_name));
}
