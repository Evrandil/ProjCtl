#include "projctl.hpp"

#include "systeminterface.hpp"


std::expected<std::string, std::string> ProjCtl::open(const std::optional<std::string>& name) const
{
	std::expected project = project_find(name);
	if (!project)
	{
		return std::unexpected(project.error());
	}

	const bool background = config.editor == "neovide" || config.editor == "emacs";

	const std::string shell_command = std::format("{} .{}", config.editor, background ? " &" : "");

	std::expected<int, std::string> result = SystemInterface::run_interactive(std::vector<std::string>{"sh", "-c", shell_command}, (*project)->second.path);

	if (!result)
	{
		return std::unexpected(std::format("{}{} {}{}", paint(config.error_color), symbol(Style::Mark::Cross), result.error(), reset()));
	}
	if (*result != 0)
	{
		return std::unexpected(std::format("{}{} {} exited with code {}{}", paint(config.error_color), symbol(Style::Mark::Cross), config.editor, *result, reset()));
	}

	return std::format("{}{}{}{}{}{}{}", paint(config.tick_color), symbol(Style::Mark::Tick), paint(config.header_color), "Opened project: ", paint(config.project_name_color), (*project)->first, reset());
}


int ProjCtl::handle(const Cmd::Base::Open& cmd) const
{
	return report(open(cmd.name));
}


