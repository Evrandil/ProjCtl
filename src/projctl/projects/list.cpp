#include "command.hpp"
#include "projctl.hpp"
#include "style.hpp"
#include "text.hpp"

#include <expected>
#include <filesystem>
#include <format>
#include <iterator>
#include <string>

auto ProjCtl::list(bool names_only) const -> std::expected<std::string, std::string> {
	const auto list_one = [&](const std::string& name, const std::filesystem::path& path) -> std::string {
		return std::format(
			"{}{:<{}}{}{:<{}}{}\n",
			paint(config.colors.name),
			truncate(name, config.widths.name),
			config.widths.name,
			paint(config.colors.path),
			truncate(path.string(), config.widths.path),
			config.widths.path,
			reset()
		);
	};

	std::string output;

	if (names_only) {
		for (const ProjectEntry& project : projects) {
			std::format_to(std::back_inserter(output), "{}\n", project.first);
		}
		return output;
	}

	if (projects.empty()) [[unlikely]] {
		return std::unexpected(
			std::format(
				"{}{}There are no projects saved{}\n",
				paint(config.colors.error),
				symbol(Style::Mark::Cross),
				reset()
			)
		);
	}

	std::format_to(
		std::back_inserter(output),
		"{}{:<{}}{}{}\n",
		paint(config.colors.header),
		"Name",
		config.widths.name,
		"Path",
		reset()
	);

	for (const ProjectEntry& project : projects) {
		std::format_to(std::back_inserter(output), "{}", list_one(project.first, project.second.path));
	}

	return output;
}

auto ProjCtl::handle(const Cmd::Base::List& cmd) const -> int {
	return cmd.gits_only ? report(list_gits(cmd.names_only)) : report(list(cmd.names_only));
}
