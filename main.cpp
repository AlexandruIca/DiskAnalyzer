#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <syslog.h>
#include <unistd.h>

#include <array>
#include <csignal>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

namespace exit_kind {

constexpr int success = EXIT_SUCCESS;
constexpr int failure = EXIT_FAILURE;

} // namespace exit_kind

namespace {

auto catch_function(int const signo) -> void
{
    syslog(LOG_ERR, "Received signal: %d", signo);
    std::exit(exit_kind::failure);
}

} // namespace

#define CATCH(sig)                                                                                                     \
    if(signal(sig, &catch_function) == SIG_ERR) {                                                                      \
        syslog(LOG_ERR, "An error occured while setting up signal handler %s!", #sig);                                 \
        return exit_kind::failure;                                                                                     \
    }                                                                                                                  \
    static_cast<void>(0)

int main()
{
    pid_t pid, sid;

    pid = fork();
    if(pid < 0) {
        std::exit(exit_kind::failure);
    }
    // If we got a good PID, then
    // we can exit the parent process
    if(pid > 0) {
        std::exit(exit_kind::success);
    }

    umask(0); // to be able to access the filesystem

    // Create a new SID for the child process
    sid = setsid();
    if(sid < 0) {
        std::exit(exit_kind::failure);
    }

    if((chdir("/")) < 0) {
        std::exit(exit_kind::failure);
    }

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    openlog("da_daemon", LOG_PID, LOG_DAEMON);

    CATCH(SIGCHLD);
    CATCH(SIGQUIT);
    CATCH(SIGILL);
    CATCH(SIGSEGV);
    CATCH(SIGTERM);
    CATCH(SIGABRT);
    CATCH(SIGFPE);
    CATCH(SIGHUP);

    syslog(LOG_NOTICE, "Work started!");

    sockaddr_in socket_address;

    int socket_fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    if(socket_fd == -1) {
        syslog(LOG_ERR, "Cannot create socket:");
        std::exit(exit_kind::failure);
    }

    std::memset(&socket_address, 0, sizeof(socket_address));

    socket_address.sin_family = AF_INET;
    socket_address.sin_port = htons(5678);
    socket_address.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(socket_fd, reinterpret_cast<sockaddr*>(&socket_address), sizeof(socket_address)) == -1) {
        syslog(LOG_ERR, "Bind failed: %s", strerror(errno));
        close(socket_fd);
        std::exit(exit_kind::failure);
    }

    constexpr int max_num_connections = 10;
    if(listen(socket_fd, max_num_connections) == -1) {
        syslog(LOG_ERR, "Listen failed: %s", strerror(errno));
        close(socket_fd);
        std::exit(exit_kind::failure);
    }

    while(true) {
        int const connect_fd = accept(socket_fd, nullptr, nullptr);

        if(connect_fd < 0) {
            syslog(LOG_ERR, "Accept failed: %s", strerror(errno));
            close(socket_fd);
            std::exit(exit_kind::failure);
        }

        syslog(LOG_NOTICE, "Accepted socket %d!", connect_fd);

        std::array<std::byte, 1024> buf;
        bool still_reading = true;

        while(still_reading) {
            ssize_t num_bytes_read = read(connect_fd, buf.data(), buf.size());

            if(num_bytes_read < 0) {
                syslog(LOG_ERR, "Couln't read from socket %d: %s", connect_fd, strerror(errno));
                close(connect_fd);
                close(socket_fd);
                std::exit(exit_kind::failure);
            }
            if(num_bytes_read == 0) {
                still_reading = false;
            }
            else {
                syslog(LOG_NOTICE, "Read %ld bytes!", num_bytes_read);
            }
        }

        if(shutdown(connect_fd, SHUT_RDWR) == -1) {
            syslog(LOG_ERR, "Shutdown failed for socket %d: %s", connect_fd, strerror(errno));
            close(connect_fd);
            close(socket_fd);
            std::exit(exit_kind::failure);
        }

        close(connect_fd);
    }

    close(socket_fd);

    syslog(LOG_NOTICE, "Work ended!");
    closelog();

    return exit_kind::success;
}
