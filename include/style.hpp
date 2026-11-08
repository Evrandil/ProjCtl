#pragma once

#include <cstdint>
#include <string_view>
#include <utility>

namespace Style
{

	enum class Color : uint8_t
	{
		Default,
		Black,
		BoldBlack,
		Red,
		BoldRed,
		Green,
		BoldGreen,
		Yellow,
		BoldYellow,
		Blue,
		BoldBlue,
		Magenta,
		BoldMagenta,
		Cyan,
		BoldCyan,
		White,
		BoldWhite,
	};

	constexpr auto color_code(const Color color) -> std::string_view
	{
		switch (color)
		{
			case Color::Default:     return "\033[39m";
			case Color::Black:       return "\033[30m";
			case Color::BoldBlack:   return "\033[1;30m";
			case Color::Red:         return "\033[31m";
			case Color::BoldRed:     return "\033[1;31m";
			case Color::Green:       return "\033[32m";
			case Color::BoldGreen:   return "\033[1;32m";
			case Color::Yellow:      return "\033[33m";
			case Color::BoldYellow:  return "\033[1;33m";
			case Color::Blue:        return "\033[34m";
			case Color::BoldBlue:    return "\033[1;34m";
			case Color::Magenta:     return "\033[35m";
			case Color::BoldMagenta: return "\033[1;35m";
			case Color::Cyan:        return "\033[36m";
			case Color::BoldCyan:    return "\033[1;36m";
			case Color::White:       return "\033[37m";
			case Color::BoldWhite:   return "\033[1;37m";
		}
		return "\033[39m";
	}

	constexpr std::string_view RESET = "\033[0m";
	constexpr std::string_view TICK  = "✓ ";
	constexpr std::string_view CROSS = "✗ ";

	enum class Mark : uint8_t
	{
		None,
		Tick,
		Cross,
	};

	constexpr auto symbol(Mark mark) -> std::string_view
	{
		switch (mark)
		{
			case Mark::None:  return "";
			case Mark::Tick:  return TICK;
			case Mark::Cross: return CROSS;
		}
		std::unreachable();
	}

} // namespace Style
