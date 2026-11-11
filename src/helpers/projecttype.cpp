#include "projecttype.hpp"

#include <filesystem>

auto detect_project_type(const std::filesystem::path& path) -> ProjectType {
	if (std::filesystem::exists(path / "Cargo.toml")) {
		return ProjectType::Rust;
	}
	if (std::filesystem::exists(path / "CMakeLists.txt")) {
		return ProjectType::CMake;
	}
	if (std::filesystem::exists(path / "package.json")) {
		return ProjectType::Node;
	}
	if (std::filesystem::exists(path / "pyproject.toml")) {
		return ProjectType::Python;
	}
	return ProjectType::Unknown;
}
