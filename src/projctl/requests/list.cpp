#include "projctl.hpp"

#include "gitinterface.hpp"
#include <ranges>
#include <future>
#include <utility>


std::string ProjCtl::request_table(const std::vector<Request>& requests) const
{
	std::string table;
	auto field_name = [&](RequestField field) -> std::string_view
	{
		switch (field)
		{
			case RequestField::Number:
				return "Number";
			case RequestField::Title:
				return "Title";
			case RequestField::State:
				return "State";
			case RequestField::Author:
				return "Author";
			case RequestField::Labels:
				return "Labels";
			case RequestField::Created:
				return "Created";
			case RequestField::Updated:
				return "Updated";
			case RequestField::Parent:
				return "Parent";
		}
		std::unreachable();
	};
	auto field_width = [&](RequestField field) -> std::size_t
	{
		switch (field)
		{
			case RequestField::Number:
				return config.request_number_width;
			case RequestField::Title:
				return config.request_title_width;
			case RequestField::State:
				return config.request_state_width;
			case RequestField::Author:
				return config.request_author_width;
			case RequestField::Labels:
				return config.request_labels_width;
			case RequestField::Created:
				[[fallthrough]];
			case RequestField::Updated:
				return 22;
			case RequestField::Parent:
				return 10;
		}
		std::unreachable();
	};
	auto field_color = [&](RequestField field, const Request& request) -> Style::Color
	{
		switch (field)
		{
			case RequestField::Number:
				return config.request_number_color;
			case RequestField::Title:
				return config.request_title_color;
			case RequestField::State:
				return request.state == "OPEN" || request.state == "opened" ? config.request_open_color : config.request_closed_color;
			case RequestField::Author:
				return config.request_author_color;
			case RequestField::Labels:
				return config.request_labels_color;
			case RequestField::Parent:
				return config.request_parent_color;
			default:
				return Style::Color::Default;
		}
	};
	table += paint(config.header_color);
	for (RequestField field : config.request_columns)
	{
		std::format_to(std::back_inserter(table), "{:<{}}", field_name(field), field_width(field));
	}
	table += reset();
	table += '\n';
	for (const Request& request : requests)
	{
		for (RequestField field : config.request_columns)
		{
			std::string value;

			switch (field)
			{
				case RequestField::Number:
					value = std::format("#{}", request.number);
					break;
				case RequestField::Title:
					value = truncate(request.title, config.request_title_width - 1);
					break;
				case RequestField::State:
					value = request.state;
					break;
				case RequestField::Author:
					value = request.author;
					break;
				case RequestField::Labels:
					table += request.labels | std::views::join_with(std::string_view{", "}) | std::ranges::to<std::string>();
					break;
				case RequestField::Created:
					value = request.created;
					break;
				case RequestField::Updated:
					value = request.updated;
					break;
				case RequestField::Parent:
					value = request.parent ? std::format("#{}", *request.parent) : "-";
					break;
			}
			std::format_to(std::back_inserter(table), "{}{:<{}}{}", paint(field_color(field, request)), truncate(value, field_width(field)), field_width(field), reset());
		}
		table += '\n';
	}
	return table;
}


std::expected<std::string, std::string> ProjCtl::requests(const ProjectEntry& project) const
{
	const ProjectContent& content = project.second;
	const std::string& project_name = project.first;

	std::expected host_option = GitInterface::host(content);
	if (!host_option)
	{
		return std::unexpected(std::format("{}{}Couldn't list requests for {}{}{}: {}", paint(config.error_color), Style::CROSS, paint(config.project_name_color), paint(config.error_color), host_option.error(), reset()));
	}
	HostOption host = *host_option;

	std::string output = std::format("{}Requests for {}{}{}:\n", paint(config.header_color), paint(config.project_name_color), project_name, reset());
	std::expected list = GitHostInterface::request_list(host, content.path.string());
	if (!list)
	{
		return std::unexpected(std::format("{}{}Couldn't list requests for {}{}{}: {}", paint(config.error_color), Style::CROSS, paint(config.project_name_color), paint(config.error_color), list.error(), reset()));
		return output;
	}
	if (list->empty())
	{
		std::format_to(std::back_inserter(output), "{}No requests{}\n", paint(config.header_color), reset());
		return output;
	}

	std::format_to(std::back_inserter(output), "{}\n", request_table(*list));

	return output;
}


std::expected<std::string, std::string> ProjCtl::requests(const std::optional<std::string>& name) const
{
	std::expected project = project_find(name);
	if (!project)
	{
		return std::unexpected(std::format("{}{}{}{}", paint(config.error_color), Style::CROSS, project.error(), reset()));
	}

	return requests(**project);
}


BatchResult ProjCtl::requests_all() const
{
	std::vector<std::future<std::expected<std::string, std::string>>> tasks;
	tasks.reserve(projects.size());

	auto is_git = [](const ProjectEntry& project) -> bool { return GitInterface::is_repo(project.second); };

	for (const auto& project : projects | std::views::filter(is_git))
	{
		tasks.push_back(std::async(std::launch::async, [this, &project]() { return requests(project); }));
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


int ProjCtl::handle(const Cmd::GH::RequestList& cmd) const
{
	return cmd.all ? report(requests_all()) : report(requests(cmd.name));
}
