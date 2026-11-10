#include "projctl.hpp"

#include "command.hpp"
#include "config.hpp"
#include "gitinterface.hpp"
#include "projectcontent.hpp"
#include "remote_url.hpp"
#include "style.hpp"
#include "text.hpp"

#include <cstdint>
#include <expected>
#include <filesystem>
#include <format>
#include <future>
#include <iterator>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

auto ProjCtl::status(const ProjectEntry& project) const -> std::string {
	constexpr std::uint8_t CONTENT_WIDTH = 24;
	std::string            output;

	const auto parse_line = [&](const std::string_view header,
	                            const Style::Mark&     mark,
	                            const std::string_view text,
	                            const Style::Color     text_color) -> void {
		Style::Color color = text_color;
		if (mark == Style::Mark::Tick) {
			color = config.colors.tick;
		} else if (mark == Style::Mark::Cross) {
			color = config.colors.cross;
		}
		std::format_to(
			std::back_inserter(output),
			"{}{}{:<{}}{}{}{}{}\n",
			MARGIN,
			paint(config.colors.header),
			header,
			config.widths.name,
			paint(color),
			symbol(mark),
			truncate(text, config.widths.max - CONTENT_WIDTH),
			reset()
		);
	};
	const auto result_line = [&](std::string_view                               header,
	                             const std::expected<std::string, std::string>& result,
	                             Style::Color                                   ok_color) -> void {
		if (result) {
			parse_line(header, Style::Mark::None, *result, ok_color);
		} else {
			parse_line(header, Style::Mark::None, result.error(), config.colors.error);
		}
	};

	const std::string&    name    = project.first;
	const ProjectContent& content = project.second;
	const bool            exists  = std::filesystem::exists(content.path);

	parse_line("Name:", Style::Mark::None, name, config.colors.name);
	parse_line("Path:", Style::Mark::None, content.path.string(), config.colors.path);

	if (!exists) {
		parse_line("Exists:", Style::Mark::Cross, "No", Style::Color::Default);
		return output;
	}

	parse_line("Exists:", Style::Mark::Tick, "Yes", Style::Color::Default);
	parse_line("Type:", Style::Mark::None, project_type_to_string(content.type), config.colors.type);

	if (!GitInterface::is_repo(content.path)) {
		parse_line("Git:", Style::Mark::Cross, "Not a git repo", config.colors.error);
		return output;
	}
	const std::expected<std::string, std::string>      remote = GitInterface::remote(content.path);
	const std::expected<std::string_view, std::string> repo =
		remote ? short_from_remote(*remote) : std::unexpected(remote.error());
	const auto repo_text = [&] -> std::string_view {
		if (repo) {
			return *repo;
		}
		if (remote) {
			return *remote;
		}
		return "No remote";
	}();
	parse_line("Git:", Style::Mark::Tick, repo_text, Style::Color::Default);
	result_line("Full remote:", remote, config.colors.remote);

	const std::expected<std::string, std::string> branch = GitInterface::branch(content.path);
	result_line("Branch", branch, config.colors.branch);

	const std::expected<std::string, std::string> changes = GitInterface::changes(content.path);
	const auto [changes_text, changes_color]              = [&] -> std::pair<std::string_view, Style::Color> {
		if (!changes) {
			return {changes.error(), config.colors.error};
		}
		if (changes->empty()) {
			return {"No changes", config.colors.tick};
		}
		return {"Changes", config.colors.warning};
	}();
	parse_line("Status:", Style::Mark::None, changes_text, changes_color);
	if (changes && !changes->empty()) {
		std::format_to(std::back_inserter(output), "\n{}{}{}\n", paint(config.colors.warning), *changes, reset());
	}

	return output;
}

auto ProjCtl::status(const std::optional<std::string>& name) const -> std::expected<std::string, std::string> {
	std::expected project = project_find(name);
	if (!project) {
		return std::unexpected(project.error());
	}
	return status(**project);
}

auto ProjCtl::status_all() const -> std::expected<std::string, std::string> {
	std::vector<std::future<std::string>> tasks;
	tasks.reserve(projects.size());

	for (const ProjectEntry& project : projects) {
		tasks.push_back(std::async(std::launch::async, [this, &project] -> std::string { return status(project); }));
	}

	std::string output;
	for (std::future<std::string>& task : tasks) {
		std::format_to(std::back_inserter(output), "{}\n\n", task.get());
	}

	return output;
}

auto ProjCtl::handle(const Cmd::Git::Status& cmd) const -> int {
	return cmd.all ? report(status_all()) : report(status(cmd.name));
}
