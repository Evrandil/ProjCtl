#pragma once

#include "githostoption.hpp"

#include <expected>
#include <string>
#include <string_view>


[[nodiscard]] std::expected<HostOption, std::string> host_from_remote(std::string_view remote);
[[nodiscard]] std::expected<std::string_view, std::string> short_from_remote(std::string_view remote);
