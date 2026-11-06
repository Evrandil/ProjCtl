#include "projctl.hpp"

#include <print>

int ProjCtl::report(const std::expected<std::string, std::string>& result) const
{
	if (!result)
	{
		std::println(stderr, "\n\n{}\n\n", result.error());
		return 1;
	}

	std::println(stdout, "\n\n{}\n\n", *result);
	return 0;
}

int ProjCtl::report(const std::expected<int, std::string>& result) const
{
	if (!result)
	{
		std::println(stderr, "\n\n{}\n\n", result.error());
		return 1;
	}

	return *result;
}

int ProjCtl::report(const BatchResult& results) const
{
	std::println(stdout, "\n\n{}\n\n", results.output);
	if (results.failed > 0)
	{
		std::println(stderr, "{}{}{}{} failed\n\n", paint(config.error_color), Style::CROSS, results.failed, reset());
		return 1;
	}
	return 0;
}

int ProjCtl::report_save(const std::expected<std::string, std::string>& result) const
{
	if (!result)
	{
		std::println(stderr, "\n\n{}\n\n", result.error());
		return 1;
	}

	std::expected saved = save();
	if (!saved)
	{
		std::println(stderr, "\n\n{}\n\n", saved.error());
		return 1;
	}

	std::println(stdout, "\n\n{}\n\n", *result);
	return 0;
}


int ProjCtl::handle(const Cmd::Unknown& cmd) const
{
	std::println(stderr, "\n\n{}{}{}{}\n\n", paint(config.error_color), Style::CROSS, cmd.message, reset());
	return 1;
}


int ProjCtl::handle(const Cmd::Base::Remove& cmd)
{
	return report_save(remove(cmd.name));
}
