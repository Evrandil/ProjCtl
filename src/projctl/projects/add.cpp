#include "projctl.hpp"

#include "command.hpp"
#include "projectcontent.hpp"
#include "style.hpp"
#include "text.hpp"

#include <expected>
#include <filesystem>
#include <format>
#include <optional>
#include <string>
#include <system_error>

auto ProjCtl::add(const std::optional<std::string>& name, const std::optional<std::filesystem::path>& path)
	-> std::expected<std::string, std::string> {
	std::error_code             error;
	const std::filesystem::path full_path = std::filesystem::weakly_canonical(
		std::filesystem::absolute(path.value_or(std::filesystem::current_path())),
		error
	);
	if (error || !std::filesystem::is_directory(full_path)) {
		return std::unexpected(
			std::format(
				"{}{}Path {}{}{} doesn't exist!{}",
				paint(config.colors.error),
				symbol(Style::Mark::Cross),
				paint(config.colors.path),
				truncate(path.value_or(".").string(), config.widths.max - MARGIN_SIZE),
				paint(config.colors.error),
				reset()
			)
		);
	}

	const std::string actual_name = name.value_or(full_path.filename().string());
	if (actual_name.empty()) {
		return std::unexpected(
			std::format(
				"{}{}Can't derive project name from {}{}{}",
				paint(config.colors.error),
				symbol(Style::Mark::Cross),
				paint(config.colors.path),
				path.value_or(".").string(),
				reset()
			)
		);
	}

	const auto [entry, inserted] = projects.try_emplace(
		actual_name,
		ProjectContent{
			.path = full_path,
		}
	);

	if (!inserted) {
		return std::unexpected(
			std::format(
				"{}{}Project with name {}{}{} already exists!{}",
				paint(config.colors.error),
				symbol(Style::Mark::Cross),
				paint(config.colors.name),
				truncate(actual_name, config.widths.max),
				paint(config.colors.error),
				reset()
			)
		);
	}

	return std::format(
		"{}{}{}{:<{}}{}{}{}\n{}{:<{}}{}{}{}",
		paint(config.colors.tick),
		symbol(Style::Mark::Tick),
		paint(config.colors.header),
		"Added project:",
		config.widths.name,
		paint(config.colors.name),
		truncate(entry->first, config.widths.max - MARGIN_SIZE),
		reset(),
		paint(config.colors.header),
		"With path:",
		config.widths.name,
		paint(config.colors.path),
		truncate(entry->second.path.string(), config.widths.max - MARGIN_SIZE),
		reset()
	);
}

auto ProjCtl::handle(const Cmd::Base::Add& cmd) -> int {
	return report_save(add(cmd.name, cmd.path));
}
