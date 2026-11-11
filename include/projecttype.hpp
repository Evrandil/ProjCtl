#pragma once

#include <cstdint>
#include <filesystem>
#include <optional>
#include <string_view>

enum class ProjectType : std::uint8_t {
	Rust,
	CMake,
	Node,
	Python,
	Zig,
	Unknown,
};

constexpr auto project_type_to_string(ProjectType type) -> std::string_view {
	switch (type) {
		case ProjectType::Rust:    return "Rust";
		case ProjectType::CMake:   return "CMake/C++";
		case ProjectType::Node:    return "Node";
		case ProjectType::Python:  return "Python";
		case ProjectType::Zig:     return "Zig";
		case ProjectType::Unknown: return "Unknown";
	}

	return "No type";
}

constexpr auto build_command_for(ProjectType type) -> std::optional<std::string_view> {
	switch (type) {
		case ProjectType::Rust:  return "cargo build 2>&1";
		case ProjectType::CMake: return "cmake -S . -B build -G Ninja 2>&1 && cmake --build build 2>&1";
		case ProjectType::Node:  return "npm run build 2>&1";
		case ProjectType::Zig:   return "zig build 2>&1";
		default:                 return std::nullopt;
	}
}

constexpr auto run_command_for(ProjectType type) -> std::optional<std::string_view> {
	switch (type) {
		case ProjectType::Rust:  return "cargo run 2>&1";
		case ProjectType::CMake: return std::nullopt;
		case ProjectType::Node:  return "npm start 2>&1";
		case ProjectType::Zig:   return "zig build run 2>&1";
		default:                 return std::nullopt;
	}
}

auto detect_project_type(const std::filesystem::path& path) -> ProjectType;
