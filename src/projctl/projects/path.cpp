#include "projctl.hpp"

#include <print>


std::expected<std::string, std::string> ProjCtl::path_show(const std::optional<std::string>& name) const
{
	std::expected project = project_find(name);
	if (!project)
	{
		return std::unexpected(project.error());
	}

	return (*project)->second.path.string();
}


int ProjCtl::handle(const Cmd::Base::Path& cmd) const
{
	std::expected<std::string, std::string> result = path_show(cmd.name);

	if (!result)
	{
		std::println(stderr, "\n\n{}\n\n", result.error());
		return 1;
	}

	std::println("{}", *result);
	return 0;
}
