#include "projctl.hpp"
#include "parser.hpp"

#include <string_view>
#include <vector>


int main(const int argc, const char **argv) {
	std::vector<std::string_view> args { argv + 1, argv + argc } ;

	Command command = parser(args);

	ProjCtl proj_ctl;

	return proj_ctl.execute(command);
}
