#ifndef LOG_HPP
#define LOG_HPP
#pragma once

#include <syslog.h>

#include <cstddef>

namespace log {

#define ERROR(...) syslog(LOG_WARNING, __VA_ARGS__)
#define INFO(...) syslog(LOG_NOTICE, __VA_ARGS__)

} // namespace log

#endif // !LOG_HPP
