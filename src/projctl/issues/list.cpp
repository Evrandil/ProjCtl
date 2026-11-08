#include "projctl.hpp"

#include "githostinterface.hpp"
#include "gitinterface.hpp"
#include <future>
#include <ranges>
#include <utility>

auto ProjCtl::issue_table(const std::vector<Issue>& issues) const -> std::string
{
	std::string table;
	auto        field_name = [&](IssueField field) -> std::string_view
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
		}
		std::unreachable();
	};
	auto field_width = [&](IssueField field) -> std::size_t
	{
		switch (field)
		{
			case IssueField::Number:
				return config.issue_number_width - 1;
			case IssueField::Title:
				return config.issue_title_width - 1;
			case IssueField::State:
				return config.issue_state_width - 1;
			case IssueField::Author:
				return config.issue_author_width - 1;
			case IssueField::Labels:
				return config.issue_labels_width - 1;
			case IssueField::Created:
				[[fallthrough]];
			case IssueField::Updated:
				return 22;
			case IssueField::Parent:
				return 10;
		}
		std::unreachable();
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
	table += paint(config.header_color);
	for (IssueField field : config.issue_columns)
	{
		std::format_to(std::back_inserter(table), "{:<{}}", field_name(field), field_width(field));
	}
	table += reset();
	table += '\n';
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
					value = issue.title;
					break;
				case IssueField::State:
					value = issue.state;
					break;
				case IssueField::Author:
					value = issue.author;
					break;
				case IssueField::Labels:
					value = issue.labels | std::views::join_with(std::string_view{", "}) | std::ranges::to<std::string>();
					break;
				case IssueField::Created:
					value = issue.created;
					break;
				case IssueField::Updated:
					value = issue.updated;
					break;
				case IssueField::Parent:
					value = issue.parent ? *issue.parent : "-";
					break;
			}

			std::format_to(std::back_inserter(table), "{}{:<{}}{}", paint(field_color(field, issue)), truncate(value, field_width(field)), field_width(field), reset());
		}
		table += '\n';
	}
	return table;
}

auto ProjCtl::issues(const ProjectEntry& project) const -> std::expected<std::string, std::string>
{
	const ProjectContent& content      = project.second;
	const std::string&    project_name = project.first;

	std::string   output = std::format("{}Issues for {}{}{}:\n", paint(config.header_color), paint(config.project_name_color), project_name, reset());
	std::expected list   = GitHostInterface::issue_list(content.path.string());
	if (!list)
	{
		return std::unexpected(
			std::format("{}{}Couldn't list issues for {}{}{}: {}", paint(config.error_color), symbol(Style::Mark::Cross), paint(config.project_name_color), paint(config.error_color), list.error(), reset()));
	}
	if (list->empty())
	{
		std::format_to(std::back_inserter(output), "{}No issues{}\n", paint(config.header_color), reset());
		return output;
	}

	std::format_to(std::back_inserter(output), "{}\n", issue_table(*list));

	return output;
}

auto ProjCtl::issues(const std::optional<std::string>& name) const -> std::expected<std::string, std::string>
{
	std::expected project = project_find(name);
	if (!project)
	{
		return std::unexpected(project.error());
	}

	return issues(**project);
}

auto ProjCtl::issues_all() const -> BatchResult
{
	std::vector<std::future<std::expected<std::string, std::string>>> tasks;
	tasks.reserve(projects.size());

	auto is_git = [](const ProjectEntry& project) -> bool { return GitInterface::is_repo(project.second.path); };

	for (const auto& project : projects | std::views::filter(is_git))
	{
		tasks.push_back(std::async(std::launch::async, [this, &project]() -> std::expected<std::string, std::string> { return issues(project); }));
	}

	BatchResult results;
	for (decltype(tasks)::value_type& task : tasks)
	{
		std::expected result = task.get();
		if (!result)
		{
			std::format_to(std::back_inserter(results.output), "{}\n", result.error());
			++results.failed;
			continue;
		}
		std::format_to(std::back_inserter(results.output), "{}\n", *result);
	}

	return results;
}

auto ProjCtl::handle(const Cmd::GH::IssueList& cmd) const -> int
{
	return cmd.all ? report(issues_all()) : report(issues(cmd.name));
}
