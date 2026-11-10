#include "projctl.hpp"

#include "command.hpp"
#include "style.hpp"
#include "systeminterface.hpp"

#include <expected>
#include <format>
#include <optional>
#include <string>
#include <vector>

auto ProjCtl::open(const std::optional<std::string>& name) const -> std::expected<std::string, std::string> {
	const std::expected project = project_find(name);
	if (!project) {
		return std::unexpected(project.error());
	}

	const bool background = config.general.editor == "neovide" || config.general.editor == "emacs";

	const std::string shell_command = std::format("{} .{}", config.general.editor, background ? " &" : "");

	const std::expected result =
		SystemInterface::run_interactive(std::vector<std::string>{"sh", "-c", shell_command}, (*project)->second.path);

	if (!result) {
		return std::unexpected(
			std::format("{}{} {}{}", paint(config.colors.error), symbol(Style::Mark::Cross), result.error(), reset())
		);
	}
	if (*result != 0) {
		return std::unexpected(
			std::format(
				"{}{} {} exited with code {}{}",
				paint(config.colors.error),
				symbol(Style::Mark::Cross),
				config.general.editor,
				*result,
				reset()
			)
		);
	}

	return std::format(
		"{}{}{}{}{}{}{}",
		paint(config.colors.tick),
		symbol(Style::Mark::Tick),
		paint(config.colors.header),
		"Opened project: ",
		paint(config.colors.name),
		(*project)->first,
		reset()
	);
}

auto ProjCtl::handle(const Cmd::Base::Open& cmd) const -> int {
	return report(open(cmd.name));
}
