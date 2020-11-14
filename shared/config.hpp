#ifndef CONFIG_HPP
#define CONFIG_HPP
#pragma once

#include <cstdlib>

namespace config {

constexpr int port = 5678;
constexpr char message_separator = '|';

} // namespace config

namespace exit_kind {

constexpr int failure = EXIT_FAILURE;
constexpr int success = EXIT_SUCCESS;

} // namespace exit_kind

#endif // !CONFIG_HPP
