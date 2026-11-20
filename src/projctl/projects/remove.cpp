#include "projctl.hpp"

#include "command.hpp"
#include "style.hpp"
#include "text.hpp"

#include <expected>
#include <format>

auto ProjCtl::handle(const Cmd::Base::Remove& cmd) -> ResultString {
	if (projects.erase(cmd.name) == 0) {
		return std::unexpected(
			std::format(
				"{}{}Project with name {}{}{} doesn't exist!{}",
				paint(config.colors.error),
				symbol(Style::Mark::Cross),
				paint(config.colors.name),
				truncate(cmd.name, config.widths.name),
				paint(config.colors.error),
				reset()
			)
		);
	}

	return with_save(
		std::format(
			"{}{}{}{:<{}}{}{}{}",
			paint(config.colors.tick),
			symbol(Style::Mark::Tick),
			paint(config.colors.header),
			"Removed project:",
			config.widths.name,
			paint(config.colors.name),
			cmd.name,
			reset()
		)
	);
}
