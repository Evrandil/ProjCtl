#include "projctl.hpp"

#include <format>
#include <print>
#include <string_view>
#include <future>
#include <vector>
#include <functional>
#include <iterator>
#include "config.hpp"
#include "systeminterface.hpp"

namespace {
	constexpr int MARGIN_SIZE = 20;
}


std::expected<std::string, std::string> ProjCtl::list() const {
	auto list_one = [&] (const std::string& name, const ProjectContent& content) {
		return std::format(
				"{}{:<{}}{}{:<{}}{}\n",
				color_code(config.project_name_color),
				truncate(name, config.name_width),
				config.name_width,
				color_code(config.project_path_color),
				truncate(content.path.string(), config.path_width),
				config.path_width,
				Style::RESET
				);
	};
	if (projects.empty()) [[unlikely]] {
		return std::unexpected(std::format(
				"{}There are no projects saved{}\n",
				color_code(config.error_color),
				Style::RESET
				));
	}

	std::vector<std::future<std::string>> tasks;
	tasks.reserve(projects.size());

	for (const decltype(projects)::value_type& project : projects) {
		tasks.push_back(std::async(
					std::launch::async,
					list_one, std::cref(project.first), std::cref(project.second)
					));
	}


	std::string output;

	std::format_to(std::back_inserter(output),
			"{}{:<{}}{}{}\n",
			color_code(config.header_color),
			"Name",
			config.name_width,
			"Path",
			Style::RESET
			);

	for (std::future<std::string>& task : tasks) {
		std::format_to(std::back_inserter(output), "{}", task.get());
	}

	return output;
}


std::expected<std::string, std::string> ProjCtl::path_show(const std::string& name) const {
	std::expected project = project_find(name);
	if (!project) {
		return std::unexpected(project.error());
	}

	return (*project)->second.path.string();
}


std::expected<std::string, std::string> ProjCtl::add(const std::string &name, std::filesystem::path path) {
	if (projects.contains(name)) {
		return std::unexpected(std::format(
					"{}Project with name {}{}{} already exists!{}",
					color_code(config.error_color),
					color_code(config.project_name_color),
					truncate(name, config.name_width),
					color_code(config.error_color),
					Style::RESET
					));
	}
	if (path.string() == ".") {
		path = std::filesystem::current_path();
	}
	std::pair<std::string, ProjectContent>  new_project = { name, ProjectContent { path, detect_project_type(path) } };
	projects.emplace(new_project);

	return std::format(
			"{}{:<{}}{}{}{}\n{}{:<{}}{}{}{}",
			color_code(config.header_color),
			"Added project:",
			config.name_width,
			color_code(config.project_name_color),
			truncate(new_project.first, config.max_width - MARGIN_SIZE),
			Style::RESET,
			color_code(config.header_color),
			"With path:",
			config.name_width,
			color_code(config.project_path_color),
			truncate(new_project.second.path.string(), config.max_width - MARGIN_SIZE),
			Style::RESET
			);
}


std::expected<std::string, std::string> ProjCtl::add_current() {
	std::filesystem::path path = std::filesystem::current_path();
	std::string name = std::filesystem::current_path().filename().string();

	if (projects.contains(name)) {
		return std::unexpected(std::format(
					"{}Project with name {}{}{} already exists!{}",
					color_code(config.error_color),
					color_code(config.project_name_color),
					truncate(name, config.name_width),
					color_code(config.error_color),
					Style::RESET
					));
	}

	std::pair<std::string, ProjectContent> new_project = { name, { path, detect_project_type(path) } };
	projects.emplace(new_project);

	return std::format(
			"{}{:<{}}{}{}{}\n{}{:<{}}{}{}{}",
			color_code(config.header_color),
			"Added project:",
			config.name_width,
			color_code(config.project_name_color),
			truncate(new_project.first, config.max_width - MARGIN_SIZE),
			Style::RESET,
			color_code(config.header_color),
			"With path:",
			config.name_width,
			color_code(config.project_path_color),
			truncate(new_project.second.path.string(), config.max_width - MARGIN_SIZE),
			Style::RESET
			);

}


std::expected<std::string, std::string> ProjCtl::remove(const std::string& name) {
	if (projects.erase(name) == 0) {
		return std::unexpected(std::format(
					"{}Project with name {}{}{} doesn't exist!{}",
					color_code(config.error_color),
					color_code(config.project_name_color),
					truncate(name, config.name_width),
					color_code(config.error_color),
					Style::RESET));
	}

	return std::format(
			"{}{:<{}}{}{}{}",
			color_code(config.header_color),
			"Removed project:",
			config.name_width,
			color_code(config.project_name_color),
			name,
			Style::RESET
			);
}




/*
void ProjCtl::open(const std::string& name) const {
	std::optional project = project_find(name);
	if (!project) {
		return;
	}

	if (config.editor == "neovide" || config.editor == "emacs") {
		SystemInterface::run_interactive(std::format(
					"cd \"{}\" && {} . &", 
					(*project)->second.path.string(), 
					config.editor
					));
	}
	else {
		SystemInterface::run_interactive(std::format(
					"cd \"{}\" && {} .",
					(*project)->second.path.string(),
					config.editor));
	}
}
*/


/*
void ProjCtl::build(const std::string& name) const {
	auto build_one = [&](const std::string& name, const ProjectContent& content){
		std::string output;
		std::format_to(std::back_inserter(output), "{}Building {}{}{}\n", color_code(config.header_color), color_code(config.project_name_color), name, Style::RESET);

		const std::optional<std::string_view> command_option = content.build_command ? content.build_command : build_command_for(content.type);
		if (!command_option) {
			std::format_to(std::back_inserter(output),"{}{}There's no default command for {}{}{} as type of project {}{}{}", color_code(config.error_color), MARGIN, color_code(config.project_type_color), project_type_to_string(content.type), color_code(config.error_color), color_code(config.project_name_color), name, Style::RESET);
			return output;
		}

		std::string command = std::format("cd \"{}\" && {}", content.path.string(), *command_option);

		std::format_to(std::back_inserter(output), "{}\n", *SystemInterface::run(command));
		return output;
	};
	if (name == "--all") {
		std::vector<std::future<std::string>> tasks;
		tasks.reserve(projects.size());
		for (const decltype(projects)::value_type& project : projects) {
			tasks.push_back(std::async(std::launch::async, build_one, std::cref(project.first), std::cref(project.second)));
		}
		for (std::future<std::string>& task : tasks) {
			println("{}", task.get());
		}
		return;
	}

	std::optional project = project_find(name);
	if (!project) {
		return;
	}
	const ProjectContent& content = (*project)->second;
	std::println("{}", build_one(name, content));
}
*/

/*
void ProjCtl::run(const std::string& name) const {
	std::optional project = project_find(name);
	if (!project) {
		return;
	}
	const ProjectContent& content = (*project)->second;

	const std::optional<std::string_view> command_option = content.run_command ? content.run_command : run_command_for(content.type);
	if (!command_option) {
		std::println("{}{}There's no default command for {}{}{} as type of project {}{}{}", color_code(config.header_color), MARGIN, color_code(config.project_type_color), project_type_to_string(content.type), color_code(config.header_color), color_code(config.project_name_color), name, Style::RESET);
		return;
	}

	std::string command = std::format("cd \"{}\" && ", content.path.string(), *command_option);

	std::println("{}", *SystemInterface::run(command));
}
*/
