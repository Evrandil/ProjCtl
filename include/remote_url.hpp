#pragma once

#include "githostoption.hpp"

#include <expected>
#include <string>
#include <string_view>

[[nodiscard]] auto host_from_remote(std::string_view remote) -> std::expected<HostOption, std::string>;
[[nodiscard]] auto short_from_remote(std::string_view remote) -> std::expected<std::string_view, std::string>;
