#include "projctl.hpp"

#include "projectcontent.hpp"
#include "text.hpp"

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <optional>
#include <string>
#include <string_view>
#include <utility>

namespace {
	auto key_of(const std::string_view entry) -> std::string_view {
		if (const std::size_t separator = entry.find('='); separator != std::string_view::npos) {
			return trim(entry.substr(0, separator));
		}
		return {};
	}

	auto value_of(const std::string_view entry) -> std::string_view {
		if (const std::size_t separator = entry.find('='); separator != std::string_view::npos) {
			return trim(entry.substr(separator + 1));
		}
		return {};
	}

	auto non_empty(const std::string_view value) -> std::optional<std::string> {
		if (value.empty()) {
			return std::nullopt;
		}
		return std::string{value};
	}

	enum class Line : std::uint8_t {
		Section,
		Path,
		Run,
		Build,
		Unknown,
	};

	auto line_of(const std::string_view line) -> Line {
		if (line.front() == '[' && line.back() == ']') {
			return Line::Section;
		}
		const std::string_view header = key_of(line);
		if (header == "path") {
			return Line::Path;
		}
		if (header == "run") {
			return Line::Run;
		}
		if (header == "build") {
			return Line::Build;
		}
		return Line::Unknown;
	}
} // namespace

void ProjCtl::load() {
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

		switch (line_of(trimmed)) {
			case Line::Section:
				if (!name.empty()) [[likely]] {
					push_new();
					clear();
				}
				name = trim(trimmed.substr(1, trimmed.size() - 2));
				continue;
			case Line::Path:    path_str = value_of(trimmed); continue;
			case Line::Run:     run_command = non_empty(value_of(trimmed)); continue;
			case Line::Build:   build_command = non_empty(value_of(trimmed)); continue;
			case Line::Unknown: continue;
		}
		std::unreachable();
	}
	if (!name.empty()) [[likely]] {
		push_new();
	}
}
