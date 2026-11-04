#include "projctl.hpp"

#include <format>
#include <string_view>
#include <future>
#include <vector>
#include <functional>
#include <iterator>
#include "config.hpp"
#include "gitinterface.hpp"


std::expected<std::string, std::string> ProjCtl::issue_list(const std::optional<std::string>& name) const
{
	auto format_issues = [&](const std::vector<Issue>& issues) -> std::string
	{
		std::string output;
		auto field_name = [&](IssueField field) -> std::string_view
		{
			switch (field)
			{
				case IssueField::Number:
					return "Number";
				case IssueField::Title:
					return "Title";
				case IssueField::State:
					return "State";
				case IssueField::Author:
					return "Author";
				case IssueField::Labels:
					return "Labels";
				case IssueField::Created:
					return "Created";
				case IssueField::Updated:
					return "Updated";
				case IssueField::Parent:
					return "Parent";
				default:
					return "";
			}
		};
		auto field_width = [&](IssueField field) -> std::size_t
		{
			switch (field)
			{
				case IssueField::Number:
					return 8;
				case IssueField::Title:
					return config.issue_title_width;
				case IssueField::State:
					return config.issue_state_width;
				case IssueField::Author:
					return config.issue_author_width;
				case IssueField::Labels:
					return 20;
				case IssueField::Created:
					return 22;
				case IssueField::Updated:
					return 22;
				case IssueField::Parent:
					return 10;
				default:
					return 0;
			}
		};
		auto field_color = [&](IssueField field, const Issue& issue) -> Style::Color
		{
			switch (field)
			{
				case IssueField::Number:
					return config.issue_number_color;
				case IssueField::Title:
					return config.issue_title_color;
				case IssueField::State:
					return issue.state == "OPEN" || issue.state == "opened" ? config.issue_open_color : config.issue_closed_color;
				case IssueField::Author:
					return config.issue_author_color;
				case IssueField::Labels:
					return config.issue_labels_color;
				case IssueField::Parent:
					return config.issue_parent_color;
				default:
					return Style::Color::Default;
			}
		};
		output += color_code(config.header_color);
		for (IssueField field : config.issue_columns)
		{
			std::format_to(std::back_inserter(output), "{:<{}}", field_name(field), field_width(field));
		}
		output += Style::RESET;
		output += '\n';
		for (const Issue& issue : issues)
		{
			for (IssueField field : config.issue_columns)
			{
				std::string value;

				switch (field)
				{
					case IssueField::Number:
						value = std::format("#{}", issue.number);
						break;
					case IssueField::Title:
						value = truncate(issue.title, config.issue_title_width - 1);
						break;
					case IssueField::State:
						value = issue.state;
						break;
					case IssueField::Author:
						value = issue.author;
						break;
					case IssueField::Labels:
						for (std::size_t i = 0; i < issue.labels.size(); ++i)
						{
							if (i != 0)
							{
								value += ", ";
							}
							value += issue.labels[i];
						}
						break;
					case IssueField::Created:
						value = issue.created;
						break;
					case IssueField::Updated:
						value = issue.updated;
						break;
					case IssueField::Parent:
						value = issue.parent ? std::format("#{}", *issue.parent) : "-";
						break;
				}
				std::format_to(std::back_inserter(output), "{}{:<{}}{}", color_code(field_color(field, issue)), value, field_width(field), Style::RESET);
			}
			output += '\n';
		}
		return output;
	};
	auto issue_one = [&](const std::string& project_name, const ProjectContent& content) -> std::string
	{
		std::expected host_option = GitInterface::host(content);
		if (!host_option)
		{
			return std::format("{}Failed to get host {}{}", color_code(config.error_color), host_option.error(), Style::RESET);
		}
		HostOption host = *host_option;

		std::string output = std::format("{}Issues for {}{}{}:\n", color_code(config.header_color), color_code(config.project_name_color), project_name, Style::RESET);
		std::expected<std::vector<Issue>, std::string> command_output = GitHostInterface::issue_list(host, content.path.string());
		if (!command_output)
		{
			std::format_to(std::back_inserter(output), "{}Failed to list issues\n{}{}", color_code(config.error_color), command_output.error(), Style::RESET);
			return output;
		}
		if (command_output->empty())
		{
			std::format_to(std::back_inserter(output), "{}No issues found{}\n", color_code(config.error_color), Style::RESET);
			return output;
		}
		std::vector<Issue> issues = *command_output;

		std::format_to(std::back_inserter(output), "{}\n", format_issues(issues));

		return output;
	};
	if (name == "--all")
	{
		std::string output;
		std::vector<std::future<std::string>> tasks;
		tasks.reserve(projects.size());
		for (const decltype(projects)::value_type& project : projects)
		{
			if (GitInterface::is_repo(project.second))
			{
				tasks.push_back(std::async(std::launch::async, issue_one, std::cref(project.first), std::cref(project.second)));
			}
		}
		for (std::future<std::string>& task : tasks)
		{
			std::format_to(std::back_inserter(output), "{}\n", task.get());
		}
		return output;
	}

	std::expected project_option = project_find(name);
	if (!project_option)
	{
		return std::unexpected(project_option.error());
	}

	const ProjectContent& content = (*project_option)->second;
	return std::format("{}\n", issue_one((*project_option)->first, content));
}


std::expected<int, std::string> ProjCtl::issue_create(const std::optional<std::string>& name) const
{
	std::expected project_option = project_find(name);
	if (!project_option)
	{
		return std::unexpected(project_option.error());
	}

	const ProjectContent& content                      = (*project_option)->second;
	std::expected<HostOption, std::string> host_option = GitInterface::host(content);
	if (!host_option)
	{
		return std::unexpected(std::format("{}Failed to get host {}{}\n", color_code(config.error_color), host_option.error(), Style::RESET));
	}
	HostOption host = *host_option;

	return GitHostInterface::issue_create(host, content.path.string());
}
