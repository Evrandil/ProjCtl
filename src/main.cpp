#include "parser.hpp"
#include "projctl.hpp"

#include <string_view>
#include <vector>

auto main(const int argc, const char** argv) -> int {
	std::vector<std::string_view> args{argv + 1, argv + argc};

	ProjCtl proj_ctl;

	return proj_ctl.execute(parser(args));
}
