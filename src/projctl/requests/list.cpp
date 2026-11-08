#include "command.hpp"
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
#include "request.hpp"
#include "style.hpp"

auto ProjCtl::request_table(const std::vector<Request>& requests) const -> std::string {
	std::string table;
	const auto  field_name = [&](RequestField field) -> std::string_view {
		switch (field) {
			case RequestField::Number:  return "Number";
			case RequestField::Title:   return "Title";
			case RequestField::State:   return "State";
			case RequestField::Author:  return "Author";
			case RequestField::Labels:  return "Labels";
			case RequestField::Created: return "Created";
			case RequestField::Updated: return "Updated";
			case RequestField::Parent:  return "Parent";
		}
		std::unreachable();
	};
	const auto field_width = [&](RequestField field) -> std::size_t {
		switch (field) {
			case RequestField::Number:  return config.request_number_width - 1;
			case RequestField::Title:   return config.request_title_width - 1;
			case RequestField::State:   return config.request_state_width - 1;
			case RequestField::Author:  return config.request_author_width - 1;
			case RequestField::Labels:  return config.request_labels_width - 1;
			case RequestField::Created: return config.request_created_width - 1;
			case RequestField::Updated: return config.request_updated_width - 1;
			case RequestField::Parent:  return config.request_parent_width - 1;
		}
		std::unreachable();
	};
	const auto field_color = [&](RequestField field, const Request& request) -> Style::Color {
		switch (field) {
			case RequestField::Number: return config.request_number_color;
			case RequestField::Title:  return config.request_title_color;
			case RequestField::State:
				if (request.state == "OPEN" || request.state == "opened") {
					return config.request_open_color;
				}
				return config.request_closed_color;
			case RequestField::Author: return config.request_author_color;
			case RequestField::Labels: return config.request_labels_color;
			case RequestField::Parent: return config.request_parent_color;
			default:                   return Style::Color::Default;
		}
	};
	table += paint(config.header_color);
	for (const RequestField field : config.request_columns) {
		std::format_to(std::back_inserter(table), "{:<{}}", field_name(field), field_width(field));
	}
	table += reset();
	table += '\n';
	for (const Request& request : requests) {
		for (const RequestField field : config.request_columns) {
			std::string value;

			switch (field) {
				case RequestField::Number: value = std::format("#{}", request.number); break;
				case RequestField::Title:  value = truncate(request.title, config.request_title_width - 1); break;
				case RequestField::State:  value = request.state; break;
				case RequestField::Author: value = request.author; break;
				case RequestField::Labels:
					table +=
						request.labels | std::views::join_with(std::string_view{", "}) | std::ranges::to<std::string>();
					break;
				case RequestField::Created: value = request.created; break;
				case RequestField::Updated: value = request.updated; break;
				case RequestField::Parent:  value = request.parent.value_or("-"); break;
			}
			std::format_to(
				std::back_inserter(table),
				"{}{:<{}}{}",
				paint(field_color(field, request)),
				truncate(value, field_width(field)),
				field_width(field),
				reset()
			);
		}
		table += '\n';
	}
	return table;
}

auto ProjCtl::requests(const ProjectEntry& project) const -> std::expected<std::string, std::string> {
	const ProjectContent& content      = project.second;
	const std::string&    project_name = project.first;

	std::string output = std::format(
		"{}Requests for {}{}{}:\n",
		paint(config.header_color),
		paint(config.project_name_color),
		project_name,
		reset()
	);
	const std::expected list = GitHostInterface::request_list(content.path.string());
	if (!list) {
		return std::unexpected(
			std::format(
				"{}{}Couldn't list requests for {}{}{}: {}",
				paint(config.error_color),
				symbol(Style::Mark::Cross),
				paint(config.project_name_color),
				paint(config.error_color),
				list.error(),
				reset()
			)
		);
	}
	if (list->empty()) {
		std::format_to(std::back_inserter(output), "{}No requests{}\n", paint(config.header_color), reset());
		return output;
	}

	std::format_to(std::back_inserter(output), "{}\n", request_table(*list));

	return output;
}

auto ProjCtl::requests(const std::optional<std::string>& name) const -> std::expected<std::string, std::string> {
	const std::expected project = project_find(name);
	if (!project) {
		return std::unexpected(project.error());
	}

	return requests(**project);
}

auto ProjCtl::requests_all() const -> BatchResult {
	std::vector<std::future<std::expected<std::string, std::string>>> tasks;
	tasks.reserve(projects.size());

	const auto is_git = [](const ProjectEntry& project) -> bool { return GitInterface::is_repo(project.second.path); };

	for (const auto& project : projects | std::views::filter(is_git)) {
		tasks.push_back(std::async(std::launch::async, [this, &project] -> std::expected<std::string, std::string> {
			return requests(project);
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

auto ProjCtl::handle(const Cmd::GH::RequestList& cmd) const -> int {
	return cmd.all ? report(requests_all()) : report(requests(cmd.name));
}
