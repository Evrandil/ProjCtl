#include "systeminterface.hpp"
#include "system_helper.hpp"

#include <array>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <expected>
#include <filesystem>
#include <format>
#include <span>
#include <spawn.h>
#include <string>
#include <sys/types.h>
#include <unistd.h>
#include <utility>
#include <vector>

auto SystemInterface::run(std::span<const std::string> args, const std::filesystem::path& cwd)
	-> std::expected<SystemInterface::Output, std::string> {
	if (args.empty()) {
		return std::unexpected("No program to run");
	}
	if (!cwd.empty() && !std::filesystem::is_directory(cwd)) {
		return std::unexpected(std::format("Directory doesn't exist: {}", cwd.string()));
	}

	std::array<int, 2> fds = {};
	if (pipe(fds.data()) != 0) {
		return std::unexpected(std::format("Couldn't create pipe: {}", std::strerror(errno)));
	}
	const int read_end  = fds.front();
	const int write_end = fds.back();

	posix_spawn_file_actions_t actions;
	posix_spawn_file_actions_init(&actions);

	posix_spawn_file_actions_adddup2(&actions, write_end, STDOUT_FILENO);
	posix_spawn_file_actions_adddup2(&actions, write_end, STDERR_FILENO);

	posix_spawn_file_actions_addclose(&actions, read_end);
	posix_spawn_file_actions_addclose(&actions, write_end);

	if (!cwd.empty()) {
		posix_spawn_file_actions_addchdir_np(&actions, cwd.c_str());
	}

	std::vector<char*> argv = SystemHelper::make_argv(args);
	pid_t              pid  = 0;

	const int error = posix_spawnp(&pid, argv.front(), &actions, nullptr, argv.data(), environ);

	posix_spawn_file_actions_destroy(&actions);
	close(write_end);

	if (error != 0) {
		close(read_end);
		return std::unexpected(std::format("Couldn't run {}: {}", args.front(), std::strerror(error)));
	}

	std::string                                     text;
	std::array<char, SystemHelper::READ_CHUNK_SIZE> buffer = {};
	while (true) {
		const ssize_t count = read(read_end, buffer.data(), buffer.size());
		if (count > 0) {
			text.append(buffer.data(), static_cast<std::size_t>(count));
			continue;
		}
		if (count == -1 && errno == EINTR) {
			continue;
		}
		break;
	}
	close(read_end);

	while (!text.empty() && text.back() == '\n') {
		text.pop_back();
	}

	return Output{.exit_code = SystemHelper::wait_for(pid), .text = std::move(text)};
}

auto SystemInterface::run_interactive(std::span<const std::string> args, const std::filesystem::path& cwd)
	-> std::expected<int, std::string> {
	if (args.empty()) {
		return std::unexpected("No program to run");
	}
	if (!cwd.empty() && !std::filesystem::is_directory(cwd)) {
		return std::unexpected(std::format("Directory doesn't exist: {}", cwd.string()));
	}

	posix_spawn_file_actions_t actions;
	posix_spawn_file_actions_init(&actions);
	if (!cwd.empty()) {
		posix_spawn_file_actions_addchdir_np(&actions, cwd.c_str());
	}

	std::vector<char*> argv  = SystemHelper::make_argv(args);
	pid_t              pid   = 0;
	const int          error = posix_spawnp(&pid, argv.front(), &actions, nullptr, argv.data(), environ);

	posix_spawn_file_actions_destroy(&actions);

	if (error != 0) {
		return std::unexpected(std::format("Couldn't run {}: {}", args.front(), std::strerror(error)));
	}

	return SystemHelper::wait_for(pid);
}
