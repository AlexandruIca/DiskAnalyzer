#include <array>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

#include "config.hpp"
#include "socket.hpp"

auto main(int const argc, char const* argv[]) noexcept -> int
{
    std::vector<std::string> args{ argv + 1, argv + argc };
    std::string to_send{};

    for(auto const& s : args) {
        to_send += s + ' ';
    }

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
