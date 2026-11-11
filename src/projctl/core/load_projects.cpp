#include "projctl.hpp"

#include "projectcontent.hpp"
#include "projects_parsing.hpp"
#include "text.hpp"
#include "ini.hpp"

#include <filesystem>
#include <fstream>
#include <optional>
#include <string>
#include <string_view>
#include <utility>

auto ProjCtl::load_projects() -> void {
	std::ifstream              projects_file{data_path};
	std::string                line;
	std::string                name;
	std::string                path_str;
	std::optional<std::string> build_command;
	std::optional<std::string> run_command;

	const auto clear = [&] -> void {
		path_str.clear();
		build_command.reset();
		run_command.reset();
	};

	const auto push_new = [&] -> void {
		projects.emplace(
			name,
			ProjectContent{
				.path          = std::filesystem::path{path_str},
				.type          = detect_project_type(path_str),
				.build_command = std::move(build_command),
				.run_command   = std::move(run_command),
			}
		);
	};

	while (std::getline(projects_file, line)) {
		const std::string_view trimmed = trim(line);
		if (trimmed.empty() || trimmed.front() == '#' || trimmed.front() == ';') {
			continue;
		}

		switch (ProjectsParsing::line_of(trimmed)) {
			case ProjectsParsing::Line::Section:
				if (!name.empty()) [[likely]] {
					push_new();
					clear();
				}
				name = trim(trimmed.substr(1, trimmed.size() - 2));
				continue;
			case ProjectsParsing::Line::Path: path_str = ProjectsParsing::value_of(trimmed); continue;
			case ProjectsParsing::Line::Run:
				run_command = ProjectsParsing::non_empty(ProjectsParsing::value_of(trimmed));
				continue;
			case ProjectsParsing::Line::Build:
				build_command = ProjectsParsing::non_empty(ProjectsParsing::value_of(trimmed));
				continue;
			case ProjectsParsing::Line::Unknown: continue;
		}
		std::unreachable();
	}
	if (!name.empty()) [[likely]] {
		push_new();
	}
}
