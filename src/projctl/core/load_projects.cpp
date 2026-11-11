#include "projctl.hpp"

#include "ini.hpp"
#include "projectcontent.hpp"

#include <filesystem>
#include <optional>
#include <string>
#include <string_view>

namespace {
	auto non_empty(const std::string_view value) -> std::optional<std::string> {
		if (value.empty()) {
			return std::nullopt;
		}
		return std::string{value};
	}
} // namespace

auto ProjCtl::load_projects() -> void {
	Ini::read(data_path, [&](std::string_view section, std::string_view key, std::string_view value) {
		if (section.empty()) {
			return;
		}
		ProjectContent& content = projects[std::string{section}];
		if (key == "path") {
			content.path = std::filesystem::path{value};
		}
		if (key == "build") {
			content.build_command = non_empty(value);
		}
		if (key == "run") {
			content.run_command = non_empty(value);
		}
	});
}
