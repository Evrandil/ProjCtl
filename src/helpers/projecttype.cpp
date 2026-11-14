#include "projecttype.hpp"

#include <filesystem>
#include <system_error>

auto detect_project_type(const std::filesystem::path& path) -> ProjectType {
	std::error_code error;
	if (std::filesystem::exists(path / "Cargo.toml", error)) {
		return ProjectType::Rust;
	}
	if (std::filesystem::exists(path / "CMakeLists.txt", error)) {
		return ProjectType::CMake;
	}
	if (std::filesystem::exists(path / "package.json", error)) {
		return ProjectType::Node;
	}
	if (std::filesystem::exists(path / "pyproject.toml", error)) {
		return ProjectType::Python;
	}
	// I don't care about errors here at all tbh, errror_code is jsut so funciton doesn't throw unexpectedly
	return ProjectType::Unknown;
}
