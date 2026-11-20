#include "projctl.hpp"

#include "command.hpp"
#include "style.hpp"
#include "version.hpp"

#include <expected>
#include <format>
#include <print>
#include <string>

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

auto ProjCtl::batch_to_string(const BatchResult& results) const -> ResultString {
	if (results.failed == 0) {
		return results.output;
	}
	return std::unexpected(std::format("{}{}{}{}{} failed", results.output, paint(config.colors.error), Style::CROSS, results.failed, reset()));
}

auto ProjCtl::with_save(const ResultString& result) const -> ResultString {
	return result.and_then([this](const std::string& message) -> ResultString {
		return save().transform([&] { return message; });
	});
}

auto ProjCtl::handle(const Cmd::Unknown& cmd) const -> ResultInt {
	return std::unexpected(
		std::format("{}{}{}{}", paint(config.colors.error), Style::CROSS, cmd.message, reset())
	);
}

auto ProjCtl::handle(const Cmd::Version& /*unused*/) const -> ResultString {
	return std::format("{}ProjCtl version: {}{}", paint(config.colors.header), Version::STRING, reset());
}
