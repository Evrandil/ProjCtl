
#include "parser.hpp"
#include "projctl.hpp"

#include <cstddef>
#include <cstdio> // Bc println has ability to throw up...
#include <cstdlib>
#include <exception>
#include <ranges>
#include <span>
#include <string_view>
#include <vector>

auto main(int argc, char** argv) -> int {
	try {
		const std::span<char*>              all_args{argv, static_cast<std::size_t>(argc)};
		const std::vector<std::string_view> args{std::from_range, all_args.subspan(1)};

		ProjCtl proj_ctl;

		return proj_ctl.execute(parser(args));
	} catch (const std::exception& error) {
		std::fputs("projctl: fatal error: ", stderr);
		std::fputs(error.what(), stderr);
		std::fputc('\n', stderr);
		return EXIT_FAILURE;
	} catch (...) {
		std::fputs("projctl: fatal error: unknown exception\n", stderr);
		return EXIT_FAILURE;
	}
}
