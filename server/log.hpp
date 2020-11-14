#ifndef LOG_HPP
#define LOG_HPP
#pragma once

#include <syslog.h>

#include <cstddef>

namespace log {

#define ERROR(...) syslog(LOG_WARNING, __VA_ARGS__)
#define INFO(...) syslog(LOG_NOTICE, __VA_ARGS__)

auto open() noexcept -> void;
auto close() noexcept -> void;

} // namespace log

#endif // !LOG_HPP
