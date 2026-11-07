#include "projctl.hpp"

#include <format>
#include <future>
#include <vector>
#include <iterator>
#include "config.hpp"
#include "gitinterface.hpp"
#include "remote_url.hpp"


std::string ProjCtl::status(const ProjectEntry& project) const
{
	constexpr uint8_t CONTENT_WIDTH = 24;
	std::string output;

	auto parse_line = [&](const std::string_view header, const std::optional<bool> tick, const std::string_view text, const Style::Color text_color)
	{
		std::string_view mark;
		Style::Color color = text_color;
		if (tick == true)
		{
			mark = Style::TICK;
			color = config.tick_color;
		}
		else if (tick == false)
		{
			mark = Style::CROSS;
			color = config.cross_color;
		}
		std::format_to(std::back_inserter(output), "{}{}{:<{}}{}{}{}{}\n", MARGIN, paint(config.header_color), header, config.name_width,
				paint(color), mark, truncate(text, config.max_width - CONTENT_WIDTH), reset());
	};
	auto result_line = [&](std::string_view header, const std::expected<std::string, std::string>& result, Style::Color ok_color)
	{
		if (result)
		{
			parse_line(header, std::nullopt, *result, ok_color);
		}
		else
		{
			parse_line(header, std::nullopt, result.error(), config.error_color);
		}
	};

	const std::string& name			= project.first;
	const ProjectContent& content	= project.second;
	const bool exists            	= std::filesystem::exists(content.path);

	parse_line("Name:", std::nullopt, name, config.project_name_color);
	parse_line("Path:", std::nullopt, content.path.string(), config.project_path_color);
	parse_line("Exists:", exists, exists ? "Yes" : "No", exists ? config.tick_color : config.cross_color);

	if (!exists)
	{
		return output;
	}
	parse_line("Type:", std::nullopt, project_type_to_string(content.type), config.project_type_color);

	if (!GitInterface::is_repo(content.path))
	{
		parse_line("Git:", false, "Not a git repo", config.cross_color);
		return output;
	}
	const std::expected<std::string, std::string> remote  = GitInterface::remote(content.path);
	const std::expected<std::string_view, std::string> repo = remote ? short_from_remote(*remote) : std::unexpected(remote.error());
	const auto [repo_text, repo_color] = [&]() -> std::pair<std::string_view, Style::Color>
	{
		if (repo)
		{
			return {*repo, config.tick_color};
		}
		if (remote)
		{
			return {*remote, config.warning_color};
		}
		return {"No remote", config.warning_color};
	}();
	parse_line("Git:", true, repo_text, repo_color);
	result_line("Full remote:", remote, config.project_remote_color);

	const std::expected<std::string, std::string> branch  = GitInterface::branch(content.path);
	result_line("Branch", branch, config.project_branch_color);

	const std::expected<std::string, std::string> changes = GitInterface::changes(content.path);
	const auto [changes_text, changes_color] = [&]() -> std::pair<std::string_view, Style::Color>
	{
		if (!changes)
		{
			return {changes.error(), config.error_color};
		}
		if (changes->empty())
		{
			return {"No changes", config.tick_color};
		}
		return {"Changes", config.warning_color};
	}();
	parse_line("Status:", std::nullopt, changes_text, changes_color);
	if (changes && !changes->empty())
	{
		std::format_to(std::back_inserter(output), "\n{}{}{}\n", paint(config.warning_color), *changes, reset());
	}

	return output;
}


std::expected<std::string, std::string> ProjCtl::status(const std::optional<std::string>& name) const
{
	std::expected project = project_find(name);
	if (!project)
	{
		return std::unexpected(std::format("{}{}{}{}", paint(config.error_color), Style::CROSS, project.error(), reset()));
	}
	return status(**project);
}

std::expected<std::string, std::string> ProjCtl::status_all() const
{
	std::vector<std::future<std::string>> tasks;
	tasks.reserve(projects.size());

	for (const ProjectEntry& project : projects)
	{
		tasks.push_back(std::async(std::launch::async, [this, &project] { return status(project); }));
	}

	std::string output;
	for (std::future<std::string>& task : tasks)
	{
		std::format_to(std::back_inserter(output), "{}\n\n", task.get());
	}

	return output;
}


int ProjCtl::handle(const Cmd::Git::Status& cmd) const
{
	return cmd.all ? report(status_all()) : report(status(cmd.name));
}
