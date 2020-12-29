#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <array>
#include <cerrno>
#include <csignal>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <deque>
#include <string>

#include "assert.hpp"
#include "log.hpp"
#include "socket.hpp"

namespace {

auto catch_function(int const signo) -> void
{
    ERROR("Received signal: %d", signo);
    std::exit(exit_kind::failure);
}

} // namespace

#define CATCH(sig)                                                                                                     \
    ASSERT_MSG((signal(sig, &catch_function) != SIG_ERR),                                                              \
               "An error occured while setting up signal handler " #sig "!");                                          \
    static_cast<void>(0)

auto close_output_handles() -> void
{
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}

auto setup_signal_handlers() -> void
{
    CATCH(SIGCHLD); // NOLINT
    CATCH(SIGQUIT); // NOLINT
    CATCH(SIGILL);  // NOLINT
    CATCH(SIGSEGV); // NOLINT
    CATCH(SIGTERM); // NOLINT
    CATCH(SIGABRT); // NOLINT
    CATCH(SIGFPE);  // NOLINT
    CATCH(SIGHUP);  // NOLINT
}

[[nodiscard]] auto analyze_dir(std::string const& path) -> std::string
{
    using namespace std::string_literals;

    std::deque<std::string> dirs{};
    std::size_t size = 0;
    std::size_t num_files = 0;

    dirent* dp = nullptr;
    DIR* dfd = nullptr;

    dirs.push_back(path);

    while(!dirs.empty()) {
        std::string const dir = dirs.front();
        dirs.pop_front();

        dfd = opendir(dir.c_str());

        if(dfd == nullptr) {
            return "Can't open"s + "'" + dir + "'";
        }

        std::string filename{};

        while(static_cast<bool>(dp = readdir(dfd))) {
            filename = dir + '/' + std::string{ static_cast<char const*>(dp->d_name) };
            struct stat stbuf = {};

            if(stat(filename.c_str(), &stbuf) < 0) {
                ERROR("Unable to stat file: %s", filename.c_str());
                continue;
            }

            if((stbuf.st_mode & S_IFMT) == S_IFDIR) {
                std::string dir_name{ static_cast<char const*>(dp->d_name) };

                if(!(dir_name == "." || dir_name == "..")) {
                    dirs.push_back(std::move(dir_name));
                }

                continue;
            }

            size += static_cast<std::size_t>(stbuf.st_size);
            ++num_files;
        }
    }

    return "Analyzed "s + std::to_string(num_files) + " files, " + std::to_string(size) + " bytes";
}

auto main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) noexcept -> int
{
    pid_t pid = 0;
    pid_t sid = 0;

    pid = fork();
    ASSERT(pid >= 0);

    // If we got a good PID, then
    // we can exit the parent process
    if(pid > 0) {
        std::exit(exit_kind::success);
    }

    umask(0); // to be able to access the filesystem

    // Create a new SID for the child process
    sid = setsid();

    ASSERT(sid >= 0);
    ASSERT(chdir("/") >= 0);

    close_output_handles();
    setup_signal_handlers();

    log::open();

    INFO("Work started!");

    net::socket srv{ net::socket::create_tcp_stream() };
    ASSERT_MSG(srv.ok(), "Cannot create socket!");
    ASSERT_MSG(srv.create_server(), "`bind` failed!");

    constexpr int max_num_connections = 10;
    ASSERT_MSG(srv.listen(max_num_connections), "`listen` failed!");

    while(true) {
        net::socket client{ srv.accept() };
        ASSERT_MSG(client.ok(), "`accept` failed!");

        INFO("Accepted socket %d!", client.get_fd());

        try {
            auto const msg = client.wait_for_message();
            INFO("Received from client: %s", msg.c_str());
            client.send_message(analyze_dir(msg));
        }
        catch(std::exception const& e) {
            INFO("%s", e.what());
        }
    }

    INFO("Work ended!");
    log::close();

    return exit_kind::success;
}
