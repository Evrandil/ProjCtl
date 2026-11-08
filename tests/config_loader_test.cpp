#include <doctest/doctest.h>

#include "config.hpp"
#include "style.hpp"
#include <expected>
#include <filesystem>
#include <fstream>

TEST_CASE("load_config") {
	const std::filesystem::path path = std::filesystem::temp_directory_path() / "projctl_test_config.ini";
	{
		std::ofstream file{path};
		file << "[Display]\n"
		     << "name_width = 33\n"
		     << "[Colors]\n"
		     << "error = cyan\n"
		     << "[Typo]\n"
		     << "name_width = 99\n";
	}

	const std::expected config = load_config(path);
	REQUIRE(config.has_value());
	CHECK(config->name_width == 33);
	CHECK(config->error_color == Style::Color::Cyan);

	std::filesystem::remove(path);
}
