#include "command.hpp"
#include "field.hpp"
#include "projctl.hpp"

#include <cstddef>
#include <expected>
#include <format>
#include <future>
#include <iterator>
#include <optional>
#include <ranges>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "githostinterface.hpp"
#include "gitinterface.hpp"
#include "projectcontent.hpp"
#include "style.hpp"

auto ProjCtl::issue_table(const std::vector<Field>& issues) const -> std::string {
	std::string table;
	const auto  field_name = [&](TableField field) -> std::string_view {
		switch (field) {
			case TableField::Number:  return "Number";
			case TableField::Title:   return "Title";
			case TableField::State:   return "State";
			case TableField::Author:  return "Author";
			case TableField::Labels:  return "Labels";
			case TableField::Created: return "Created";
			case TableField::Updated: return "Updated";
			case TableField::Parent:  return "Parent";
		}
		std::unreachable();
	};
	const auto field_width = [&](TableField field) -> std::size_t {
		switch (field) {
			case TableField::Number:  return config.tables.number.width;
			case TableField::Title:   return config.tables.title.width;
			case TableField::State:   return config.tables.state.width;
			case TableField::Author:  return config.tables.author.width;
			case TableField::Labels:  return config.tables.labels.width;
			case TableField::Created: return config.tables.created.width;
			case TableField::Updated: return config.tables.updated.width;
			case TableField::Parent:  return config.tables.parent.width;
		}
		std::unreachable();
	};
	const auto field_color = [&](TableField field, const Field& issue) -> Style::Color {
		switch (field) {
			case TableField::Number: return config.tables.number.color;
			case TableField::Title:  return config.tables.title.color;
			case TableField::State:
				if (issue.state == "OPEN" || issue.state == "opened") {
					return config.colors.open;
				}
				return config.colors.closed;
			case TableField::Author: return config.tables.author.color;
			case TableField::Labels: return config.tables.labels.color;
			case TableField::Parent: return config.tables.parent.color;
			default:                 return Style::Color::Default;
		}
	};
	table += paint(config.colors.header);
	for (const TableField field : config.tables.issue_columns) {
		std::format_to(std::back_inserter(table), "{:<{}}", field_name(field), field_width(field));
	}
	table += reset();
	table += '\n';
	for (const Field& issue : issues) {
		for (const TableField field : config.tables.issue_columns) {
			std::string value;

			switch (field) {
				case TableField::Number: value = std::format("#{}", issue.number); break;
				case TableField::Title:  value = issue.title; break;
				case TableField::State:  value = issue.state; break;
				case TableField::Author: value = issue.author; break;
				case TableField::Labels:
					value =
						issue.labels | std::views::join_with(std::string_view{", "}) | std::ranges::to<std::string>();
					break;
				case TableField::Created: value = issue.created; break;
				case TableField::Updated: value = issue.updated; break;
				case TableField::Parent:  value = issue.parent.value_or("-"); break;
			}

			std::format_to(
				std::back_inserter(table),
				"{}{:<{}}{}",
				paint(field_color(field, issue)),
				truncate(value, field_width(field)),
				field_width(field),
				reset()
			);
		}
		table += '\n';
	}
	return table;
}

auto ProjCtl::issues(const ProjectEntry& project) const -> std::expected<std::string, std::string> {
	const ProjectContent& content      = project.second;
	const std::string&    project_name = project.first;

	std::string output = std::format(
		"{}Issues for {}{}{}:\n",
		paint(config.colors.header),
		paint(config.colors.name),
		project_name,
		reset()
	);
	const std::expected list = GitHostInterface::issue_list(content.path.string());
	if (!list) {
		return std::unexpected(
			std::format(
				"{}{}Couldn't list issues for {}{}{}: {}{}",
				paint(config.colors.error),
				symbol(Style::Mark::Cross),
				paint(config.colors.name),
				project_name,
				paint(config.colors.error),
				list.error(),
				reset()
			)
		);
	}
	if (list->empty()) {
		std::format_to(std::back_inserter(output), "{}No issues{}\n", paint(config.colors.header), reset());
		return output;
	}

	std::format_to(std::back_inserter(output), "{}\n", issue_table(*list));

	return output;
}

auto ProjCtl::issues(const std::optional<std::string>& name) const -> std::expected<std::string, std::string> {
	const std::expected project = project_find(name);
	if (!project) {
		return std::unexpected(project.error());
	}

	return issues(**project);
}

auto ProjCtl::issues_all() const -> BatchResult {
	std::vector<std::future<std::expected<std::string, std::string>>> tasks;
	tasks.reserve(projects.size());

	const auto is_git = [](const ProjectEntry& project) -> bool { return GitInterface::is_repo(project.second.path); };

	for (const auto& project : projects | std::views::filter(is_git)) {
		tasks.push_back(std::async(std::launch::async, [this, &project] -> std::expected<std::string, std::string> {
			return issues(project);
		}));
	}

	BatchResult results;
	for (decltype(tasks)::value_type& task : tasks) {
		const std::expected result = task.get();
		if (!result) {
			std::format_to(std::back_inserter(results.output), "{}\n", result.error());
			++results.failed;
			continue;
		}
		std::format_to(std::back_inserter(results.output), "{}\n", *result);
	}

	return results;
}

auto ProjCtl::handle(const Cmd::GH::IssueList& cmd) const -> int {
	return cmd.all ? report(issues_all()) : report(issues(cmd.name));
}
