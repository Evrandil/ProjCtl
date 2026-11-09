#include "command.hpp"
#include "projctl.hpp"
#include "style.hpp"
#include <expected>
#include <format>
#include <string>

auto ProjCtl::remove(const std::string& name) -> std::expected<std::string, std::string> {
	if (projects.erase(name) == 0) {
		return std::unexpected(
			std::format(
				"{}{}Project with name {}{}{} doesn't exist!{}",
				paint(config.colors.error),
				symbol(Style::Mark::Cross),
				paint(config.colors.name),
				truncate(name, config.widths.name),
				paint(config.colors.error),
				reset()
			)
		);
	}

	return std::format(
		"{}{}{}{:<{}}{}{}{}",
		paint(config.colors.tick),
		symbol(Style::Mark::Tick),
		paint(config.colors.header),
		"Removed project:",
		config.widths.name,
		paint(config.colors.name),
		name,
		reset()
	);
}

auto ProjCtl::handle(const Cmd::Base::Remove& cmd) -> int {
	return report_save(remove(cmd.name));
}
