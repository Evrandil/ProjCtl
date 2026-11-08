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
				paint(config.error_color),
				symbol(Style::Mark::Cross),
				paint(config.project_name_color),
				truncate(name, config.name_width),
				paint(config.error_color),
				reset()
			)
		);
	}

	return std::format(
		"{}{}{}{:<{}}{}{}{}",
		paint(config.tick_color),
		symbol(Style::Mark::Tick),
		paint(config.header_color),
		"Removed project:",
		config.name_width,
		paint(config.project_name_color),
		name,
		reset()
	);
}

auto ProjCtl::handle(const Cmd::Base::Remove& cmd) -> int {
	return report_save(remove(cmd.name));
}
