#include "projctl.hpp"

#include "githostinterface.hpp"

auto ProjCtl::request_merge(const std::optional<std::string>& name) const -> std::expected<std::string, std::string>
{
	std::expected project = project_find(name);
	if (!project)
	{
		return std::unexpected(project.error());
	}

	std::expected result = GitHostInterface::request_merge((*project)->second.path.string());
	if (!result)
	{
		return std::unexpected(std::format("{}{}{}{}", paint(config.error_color), symbol(Style::Mark::Cross), result.error(), reset()));
	}
	return std::format("{}{}{}{}", paint(config.tick_color), Style::TICK, *result, reset());
}

auto ProjCtl::handle(const Cmd::GH::RequestMerge& cmd) const -> int
{
	return report(request_merge(cmd.name));
}
