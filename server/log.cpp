#include "log.hpp"

namespace log {

auto open() noexcept -> void
{
    ::openlog("da_daemon", LOG_PID, LOG_DAEMON);
}

auto close() noexcept -> void
{
    ::closelog();
}

} // namespace log
