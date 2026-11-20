#include "projctl.hpp"

#include "command.hpp"
#include "style.hpp"
#include "version.hpp"

#include <expected>
#include <format>
#include <print>
#include <string>
#include <variant>

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

auto ProjCtl::report(const BatchResult& result) -> int {
	if (result.failed == 0) {
		println(stdout, "\n\n{}\n\n", result.output);
		return 0;
	}
	println(stderr,
			"{}{}{}{}{} failed",
			result.output,
			paint(config.colors.error),
			Style::CROSS,
			result.failed,
			reset()
		);
	return 1;
}

auto ProjCtl::with_save(const ResultString& result) const -> ResultString {
	return result.and_then([this](const std::string& message) -> ResultString {
		return save().transform([&] { return message; });
	});
}

auto ProjCtl::handle(const Cmd::Unknown& cmd) const -> ResultInt {
	return std::unexpected(std::format("{}{}{}{}", paint(config.colors.error), Style::CROSS, cmd.message, reset()));
}

auto ProjCtl::handle(const Cmd::Version& /*unused*/) const -> ResultString {
	return std::format("{}ProjCtl version: {}{}", paint(config.colors.header), Version::STRING, reset());
}
