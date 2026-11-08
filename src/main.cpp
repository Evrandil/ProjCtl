#include <string_view>
#include <vector>
#include <span>
#include <ranges>
#include <print>
#include <cstdlib>
#include <cstddef>

#include "parser.hpp"
#include "projctl.hpp"

auto main(int argc, char** argv) -> int {
	try {
		const std::span<char*> all_args{argv, static_cast<size_t>(argc)};
		const std::vector<std::string_view> args{std::from_range, all_args.subspan(1)};

		ProjCtl proj_ctl;

		return proj_ctl.execute(parser(args));
	}
	catch (const std::exception& error) {
		std::println(stderr, "projctl: fatal error: {}", error.what());
		return EXIT_FAILURE;
	}
	catch (...) {
		std::println(stderr, "projctl: fatal error: unknown exception");
		return EXIT_FAILURE;
	}
}
