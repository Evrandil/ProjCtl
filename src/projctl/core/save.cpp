#include "projctl.hpp"

#include <fstream>

auto ProjCtl::save() const noexcept -> std::expected<void, std::string> {
	std::error_code error;
	std::filesystem::create_directories(data_path.parent_path(), error);

	if (error) {
		return std::unexpected(
			std::format(
				"{}{}Couldn't create {}{}{}: {}{}",
				paint(config.error_color),
				Style::CROSS,
				paint(config.project_path_color),
				data_path.parent_path().string(),
				paint(config.error_color),
				error.message(),
				reset()
			)
		);
	}

	std::filesystem::path temp_path = data_path;
	temp_path += ".tmp";

	{
		std::ofstream file{temp_path};

		for (const ProjectEntry& project : projects) {
			file << '[' << project.first << "]\n";
			file << "path=" << project.second.path.string() << '\n';

			if (project.second.build_command) {
				file << "build=" << *project.second.build_command << '\n';
			}
			if (project.second.run_command) {
				file << "run=" << *project.second.run_command << '\n';
			}

			file << '\n';
		}
		file.close();
		if (!file) {
			std::filesystem::remove(temp_path, error);
			return std::unexpected(
				std::format(
					"{}{}Couldn't write to: {}{}{}",
					paint(config.error_color),
					Style::CROSS,
					paint(config.project_path_color),
					temp_path.string(),
					reset()
				)
			);
		}
	}

	std::filesystem::rename(temp_path, data_path, error);
	if (error) {
		return std::unexpected(
			std::format(
				"{}{}Couldn't save {}{}{}: {}{}",
				paint(config.error_color),
				Style::CROSS,
				paint(config.project_path_color),
				data_path.string(),
				paint(config.error_color),
				error.message(),
				reset()
			)
		);
	}
	return {};
}
