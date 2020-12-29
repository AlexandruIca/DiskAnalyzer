#include <array>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

#include <dirent.h>
#include <unistd.h>

#include "config.hpp"
#include "socket.hpp"

[[nodiscard]] auto get_absolute_path(std::string const& path) -> std::string
{
    std::array<char, PATH_MAX + 1> abs_dir{};
    realpath(path.c_str(), abs_dir.data());
    return std::string{ abs_dir.data() };
}

auto main(int const argc, char const* argv[]) noexcept -> int
{
    std::vector<std::string> args{ argv + 1, argv + argc };

    if(args.empty()) {
        std::cout << "Please specify a directory to be analyzed!" << std::endl;
        return 0;
    }

    std::string to_send{ get_absolute_path(args.front()) };
    net::socket connection{ net::socket::create_tcp_stream() };

    if(!connection.ok()) {
        perror("Can't create socket!");
        std::exit(exit_kind::failure);
    }

    if(!connection.connect_to_server()) {
        perror("`connect` failed!");
        std::exit(exit_kind::failure);
    }

    try {
        connection.send_message(to_send);
        auto const received = connection.wait_for_message();
        std::cout << "Received from server: " << received << std::endl;
    }
    catch(std::exception const& e) {
        std::cout << e.what() << std::endl;
    }

    return exit_kind::success;
}
