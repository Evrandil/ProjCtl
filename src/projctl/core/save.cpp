#include "projctl.hpp"

#include <fstream>


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
