#include "projctl.hpp"

#include "config.hpp"
#include "gitinterface.hpp"
#include "remote_url.hpp"
#include <format>
#include <future>
#include <iterator>
#include <vector>

auto ProjCtl::status(const ProjectEntry& project) const -> std::string {
	constexpr uint8_t CONTENT_WIDTH = 24;
	std::string       output;

	auto parse_line = [&](const std::string_view header,
	                      const Style::Mark&     mark,
	                      const std::string_view text,
	                      const Style::Color     text_color) -> void {
		Style::Color color = text_color;
		if (mark == Style::Mark::Tick) {
			color = config.tick_color;
		} else if (mark == Style::Mark::Cross) {
			color = config.cross_color;
		}
		std::format_to(
			std::back_inserter(output),
			"{}{}{:<{}}{}{}{}{}\n",
			MARGIN,
			paint(config.header_color),
			header,
			config.name_width,
			paint(color),
			symbol(mark),
			truncate(text, config.max_width - CONTENT_WIDTH),
			reset()
		);
	};
	auto result_line = [&](std::string_view                               header,
	                       const std::expected<std::string, std::string>& result,
	                       Style::Color                                   ok_color) -> void {
		if (result) {
			parse_line(header, Style::Mark::None, *result, ok_color);
		} else {
			parse_line(header, Style::Mark::None, result.error(), config.error_color);
		}
	};

	const std::string&    name    = project.first;
	const ProjectContent& content = project.second;
	const bool            exists  = std::filesystem::exists(content.path);

	parse_line("Name:", Style::Mark::None, name, config.project_name_color);
	parse_line("Path:", Style::Mark::None, content.path.string(), config.project_path_color);

	if (!exists) {
		parse_line("Exists:", Style::Mark::Cross, "No", Style::Color::Default);
		return output;
	}

	parse_line("Exists:", Style::Mark::Tick, "Yes", Style::Color::Default);
	parse_line("Type:", Style::Mark::None, project_type_to_string(content.type), config.project_type_color);

	if (!GitInterface::is_repo(content.path)) {
		parse_line("Git:", Style::Mark::Cross, "Not a git repo", config.cross_color);
		return output;
	}
	const std::expected<std::string, std::string>      remote = GitInterface::remote(content.path);
	const std::expected<std::string_view, std::string> repo =
		remote ? short_from_remote(*remote) : std::unexpected(remote.error());
	const auto repo_text = [&]() -> std::string_view {
		if (repo) {
			return *repo;
		}
		if (remote) {
			return *remote;
		}
		return "No remote";
	}();
	parse_line("Git:", Style::Mark::Tick, repo_text, Style::Color::Default);
	result_line("Full remote:", remote, config.project_remote_color);

	const std::expected<std::string, std::string> branch = GitInterface::branch(content.path);
	result_line("Branch", branch, config.project_branch_color);

	const std::expected<std::string, std::string> changes = GitInterface::changes(content.path);
	const auto [changes_text, changes_color]              = [&]() -> std::pair<std::string_view, Style::Color> {
		if (!changes) {
			return {changes.error(), config.error_color};
		}
		if (changes->empty()) {
			return {"No changes", config.tick_color};
		}
		return {"Changes", config.warning_color};
	}();
	parse_line("Status:", Style::Mark::None, changes_text, changes_color);
	if (changes && !changes->empty()) {
		std::format_to(std::back_inserter(output), "\n{}{}{}\n", paint(config.warning_color), *changes, reset());
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
