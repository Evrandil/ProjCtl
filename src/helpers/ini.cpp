#include "ini.hpp"

#include "text.hpp"

#include <cstddef>
#include <filesystem>
#include <fstream>
#include <istream>
#include <string>
#include <string_view>

auto Ini::parse(std::istream& input, const Handler& handle) -> void {
	std::string section;
	std::string line;
	while (getline(input, line)) {
		const std::string_view trimmed = trim(line);
		if (trimmed.empty() || trimmed.front() == '#' || trimmed.front() == ';') {
			continue;
		}
		if (trimmed.front() == '[') {
			if (trimmed.back() == ']') {
				section = trim(trimmed.substr(1, trimmed.size() - 2));
				continue;
			}
			section = std::string_view{};
			continue;
		}
		const std::size_t separator = trimmed.find('=');
		if (separator == std::string_view::npos) {
			continue;
		}
		const std::string_view key = trim(trimmed.substr(0, separator));
		const std::string_view value = trim(trimmed.substr(separator + 1));
		handle(section, key, value);
	}
}

auto Ini::read(const std::filesystem::path& path, const Handler& handle) -> bool {
	std::ifstream file{path};
	if (!file) {
		return false;
	}
	parse(file, handle);
	return true;
}
