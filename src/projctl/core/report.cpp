#include "command.hpp"
#include "projctl.hpp"

#include <expected>
#include <print>

#include "style.hpp"
#include "version.hpp"

auto ProjCtl::report(const ResultString& result) -> int {
	if (!result) {
		std::println(stderr, "\n\n{}\n\n", result.error());
		return 1;
	}

	std::println(stdout, "\n\n{}\n\n", *result);
	return 0;
}

auto ProjCtl::report(const ResultInt& result) -> int {
	if (!result) {
		std::println(stderr, "\n\n{}\n\n", result.error());
		return 1;
	}

	return *result;
}

auto ProjCtl::report(const BatchResult& results) const -> int {
	std::println(stdout, "\n\n{}\n\n", results.output);
	if (results.failed > 0) {
		std::println(stderr, "{}{}{}{} failed\n\n", paint(config.colors.error), Style::CROSS, results.failed, reset());
		return 1;
	}
	return 0;
}

auto ProjCtl::report_save(const ResultString& result) const -> int {
	if (!result) {
		std::println(stderr, "\n\n{}\n\n", result.error());
		return 1;
	}

	std::expected saved = save();
	if (!saved) {
		std::println(stderr, "\n\n{}\n\n", saved.error());
		return 1;
	}

	std::println(stdout, "\n\n{}\n\n", *result);
	return 0;
}

auto ProjCtl::handle(const Cmd::Unknown& cmd) const -> int {
	std::println(stderr, "\n\n{}{}{}{}\n\n", paint(config.colors.error), Style::CROSS, cmd.message, reset());
	return 1;
}

auto ProjCtl::handle(const Cmd::Version& /*unused*/) const -> int {
	std::println(stdout, "\n\n{}ProjCtl version: {}{}\n\n", paint(config.colors.header), Version::STRING, reset());
	return 0;
}
