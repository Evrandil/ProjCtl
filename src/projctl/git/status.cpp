#include "projctl.hpp"

#include <format>
#include <future>
#include <vector>
#include <iterator>
#include "config.hpp"
#include "gitinterface.hpp"


std::string ProjCtl::status(const ProjectEntry& project) const
{
	constexpr uint8_t CONTENT_WIDTH = 24;
	std::string output;

	auto parse_line = [&](const std::string_view header, const std::optional<bool> tick, const std::string_view text, const Style::Color text_color)
	{
		std::format_to(std::back_inserter(output), "{}{}{:<{}}{}{}{}{}\n", MARGIN, color_code(config.header_color), header, config.name_width,
					   tick ? *tick ? color_code(config.tick_color) : color_code(config.cross_color) : color_code(text_color), tick ? *tick ? Style::TICK : Style::CROSS : "",
					   truncate(text, config.max_width - CONTENT_WIDTH), Style::RESET);
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
	const std::expected<std::string, std::string> git = GitInterface::remote_short(content);
	parse_line("Git:", git.has_value(), git ? *git : git.error(), git ? config.tick_color : config.cross_color);

	if (!git)
	{
		return output;
	}
	const std::expected<std::string, std::string> branch  = GitInterface::branch(content);
	const std::expected<std::string, std::string> remote  = GitInterface::remote(content);
	const std::expected<std::string, std::string> changes = GitInterface::changes(content);
	parse_line("Branch:", std::nullopt, branch ? *branch : branch.error(), branch ? config.project_branch_color : config.cross_color);
	parse_line("Full remote:", std::nullopt, remote ? *remote : remote.error(), remote ? config.project_remote_color : config.cross_color);
	parse_line("Status:", std::nullopt, changes ? changes->empty() ? "No changes" : "Changes:" : changes.error(),
			changes ? changes->empty() ? config.tick_color : config.warning_color : config.error_color);
	if (changes && !changes->empty())
	{
		std::format_to(std::back_inserter(output), "\n{}{}{}\n", color_code(config.warning_color), *changes, Style::RESET);
	}

	return output;
}


std::expected<std::string, std::string> ProjCtl::status(const std::optional<std::string>& name) const
{
	std::expected project = project_find(name);
	if (!project)
	{
		return std::unexpected(std::format("{}{}{}{}", color_code(config.error_color), Style::CROSS, project.error(), Style::RESET));
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
