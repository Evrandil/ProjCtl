#pragma once

#include <expected>
#include <string>
#include <string_view>

#include "githostoption.hpp"

[[nodiscard]] auto host_from_remote(std::string_view remote) -> std::expected<HostOption, std::string>;
[[nodiscard]] auto short_from_remote(std::string_view remote) -> std::expected<std::string_view, std::string>;
