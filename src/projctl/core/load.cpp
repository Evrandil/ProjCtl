#include "projctl.hpp"

#include <fstream>

void ProjCtl::load() {
	config.load(config_path);

	std::ifstream projects_file{data_path};
	std::string   line;
	std::string   name;
	std::string   path_str;
	std::string   build_command;
	std::string   run_command;
	std::size_t   separator = 0;

	while (std::getline(projects_file, line)) {
		line = trim(line);
		if (line.empty() || line.front() == '#' || line.front() == ';') {
			continue;
		}

		switch (line.front()) {
			case '[':
				if (!name.empty()) [[likely]] {
					this->projects.emplace(
						name,
						ProjectContent{
							.path = std::filesystem::path{path_str},
							.type = detect_project_type(path_str),
							.build_command = build_command.empty() ? std::nullopt : std::optional<std::string>{build_command},
							.run_command = run_command.empty() ? std::nullopt : std::optional<std::string>{run_command},
						}
					);
					path_str.clear();
					build_command.clear();
					run_command.clear();
				}
				name = trim(line.substr(1, line.size() - 2));
				continue;
			case 'p':
				separator = line.find('=');
				path_str  = trim(line.substr(separator + 1));
				continue;
			case 'r':
				separator   = line.find('=');
				run_command = trim(line.substr(separator + 1));
				continue;
			case 'b':
				separator     = line.find('=');
				build_command = trim(line.substr(separator + 1));
				continue;
			default: continue;
		}
	}
	if (!name.empty()) [[likely]] {
		this->projects.emplace(
			name,
			ProjectContent{
				.path = std::filesystem::path{path_str},
				.type = detect_project_type(path_str),
				.build_command = build_command.empty() ? std::nullopt : std::optional<std::string>{build_command},
				.run_command = run_command.empty() ? std::nullopt : std::optional<std::string>{run_command},
			}
		);
	}
}
