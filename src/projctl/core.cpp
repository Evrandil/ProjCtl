#include "projctl.hpp"

#include <format>
#include <fstream>
#include "config.hpp"
#include "systeminterface.hpp"


ProjCtl::ProjCtl()
{
	config_path = SystemInterface::config_home() / "projctl/config.ini";
	data_path   = SystemInterface::data_home() / "projctl/projects.ini";
	load();
}


void ProjCtl::load()
{
	config.load(config_path);

	std::ifstream projects_file{data_path};
	std::string line;
	std::string name;
	std::string path_str;
	std::string build_command;
	std::string run_command;
	std::size_t separator = 0;

	while (std::getline(projects_file, line))
	{
		line = trim(line);
		if (line.empty() || line.front() == '#' || line.front() == ';')
		{
			continue;
		}

		switch (line.front())
		{
			case '[':
				if (!name.empty()) [[likely]]
				{
					this->projects.emplace(name, ProjectContent{
													 std::filesystem::path{path_str},
													 detect_project_type(path_str),
													 build_command.empty() ? std::nullopt : std::optional<std::string>{build_command},
													 run_command.empty() ? std::nullopt : std::optional<std::string>{run_command},
												 });
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
			default:
				continue;
		}
	}
	if (!name.empty()) [[likely]]
	{
		this->projects.emplace(name, ProjectContent{
										 std::filesystem::path{path_str},
										 detect_project_type(path_str),
										 build_command.empty() ? std::nullopt : std::optional<std::string>{build_command},
										 run_command.empty() ? std::nullopt : std::optional<std::string>{run_command},
									 });
	}
}


std::expected<void, std::string> ProjCtl::save() const noexcept
{
	std::error_code error;
	std::filesystem::create_directories(data_path.parent_path(), error);

	if (error)
	{
		return std::unexpected(std::format("Couldn't create {}: {}", data_path.parent_path().string(), error.message()));
	}

	std::filesystem::path temp_path = data_path;
	temp_path += ".tmp";

	{
		std::ofstream file{temp_path};

		for (const decltype(projects)::value_type& project : projects)
		{
			file << '[' << project.first << "]\n";
			file << "path=" << project.second.path.string() << '\n';

			if (project.second.build_command)
			{
				file << "build=" << *project.second.build_command << '\n';
			}
			if (project.second.run_command)
			{
				file << "run=" << *project.second.run_command << '\n';
			}

			file << '\n';
		}
		file.close();
		if (!file)
		{
			std::filesystem::remove(temp_path, error);
			return std::unexpected(std::format("Couldn't write to: {}", temp_path.string()));
		}
	}

	std::filesystem::rename(temp_path, data_path, error);
	if (error)
	{
		return std::unexpected(std::format("Couldn't save {}: {}", data_path.string(), error.message()));
	}
	return {};
}


std::optional<std::string> ProjCtl::current_project_name() const
{
	std::filesystem::path cwd = std::filesystem::weakly_canonical(std::filesystem::current_path());
	for (const decltype(projects)::value_type& project : projects)
	{
		std::filesystem::path project_path = std::filesystem::weakly_canonical(project.second.path);

		if (cwd == project_path)
		{
			return project.first;
		}
	}
	return std::nullopt;
}


std::expected<ProjectIterator, std::string> ProjCtl::project_find(const std::optional<std::string>& name) const
{
	std::string actual_name;
	if (name)
	{
		actual_name = *name;
	}
	else
	{
		std::optional<std::string> current = current_project_name();
		if (!current)
		{
			return std::unexpected("Current directory isn't a saved project");
		}
		actual_name = *current;
	}
	std::map<std::string, ProjectContent>::const_iterator project = projects.find(actual_name);
	if (project == projects.end())
	{
		return std::unexpected(std::format("Project with name {} doesn't exist", actual_name));
	}
	return project;
}
