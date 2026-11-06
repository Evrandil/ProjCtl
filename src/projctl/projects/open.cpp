#include "projctl.hpp"

#include "systeminterface.hpp"


std::expected<std::string, std::string> ProjCtl::open(const std::optional<std::string>& name) const
{
	std::expected project = project_find(name);
	if (!project)
	{
		return std::unexpected(std::format("{}{}{}{}", paint(config.error_color), Style::CROSS, project.error(), reset()));
	}

	const bool background = config.editor == "neovide" || config.editor == "emacs";

	const std::string shell_command = std::format("{} .{}", config.editor, background ? " &" : "");

	std::expected<int, std::string> result = SystemInterface::run_interactive(std::vector<std::string>{"sh", "-c", shell_command}, (*project)->second.path);

	if (!result)
	{
		return std::unexpected(result.error());
	}
	if (*result != 0)
	{
		return std::unexpected(std::format("{} exited with code {}", config.editor, *result));
	}

	return std::format("{}{}{}{}{}", paint(config.header_color), "Opened project: ", paint(config.project_name_color), (*project)->first, reset());
}


int ProjCtl::handle(const Cmd::Base::Open& cmd) const
{
	return report(open(cmd.name));
}


