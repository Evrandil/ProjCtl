#pragma once

#include <filesystem>
#include <functional>
#include <istream>
#include <string_view>


namespace Ini {
	using Handler = std::function<void(std::string_view section, std::string_view key, std::string_view value)>;

	auto parse(std::istream& input, const Handler& handle) -> void;
	auto read(const std::filesystem::path& path, const Handler& handle) -> bool;
}
