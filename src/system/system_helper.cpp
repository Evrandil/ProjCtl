#include "system_helper.hpp"

#include <cerrno>
#include <span>
#include <stdlib.h> // NOLINT(modernize-deprecated-headers)
#include <string>
#include <sys/types.h>
#include <sys/wait.h> // IWYU pragma: keep
#include <vector>

auto make_argv(std::span<const std::string> args) -> std::vector<char*> {
	std::vector<char*> argv;
	argv.reserve(args.size() + 1);
	for (const std::string& arg : args) {
		argv.push_back(const_cast<char*>(arg.c_str())); // NOLINT(cppcoreguidelines-pro-type-const-cast)
	}
	argv.push_back(nullptr);
	return argv;
}

auto wait_for(pid_t pid) -> int {
	int status = 0;
	while (waitpid(pid, &status, 0) == -1) {
		if (errno != EINTR) {
			return 1;
		}
	}

	if (WIFEXITED(status)) {
		return WEXITSTATUS(status);
	}
	if (WIFSIGNALED(status)) {
		return SIGNAL_EXIT_BASE + WTERMSIG(status);
	}
	return 1;
}
