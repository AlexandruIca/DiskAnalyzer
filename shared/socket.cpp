#include "socket.hpp"
#include "config.hpp"

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstring>
#include <utility>

namespace net {

socket_exception::socket_exception(std::string msg, int const fd)
    : m_msg{ std::move(msg) }
{
    using namespace std::string_literals;
    m_msg = "Socket "s + std::to_string(fd) + " exception: "s + m_msg;
}

auto socket_exception::what() const noexcept -> char const*
{
    return m_msg.c_str();
}

socket::socket(int const fd)
    : m_socket_fd{ fd }
{
}

socket::~socket() noexcept
{
    close(m_socket_fd);
}

auto socket::get_fd() const noexcept -> int
{
    return m_socket_fd;
}

auto socket::ok() const noexcept -> bool
{
    return m_socket_fd >= 0;
}

auto socket::create_server() const noexcept -> bool
{
    sockaddr_in socket_address{};
    std::memset(&socket_address, 0, sizeof(socket_address));
    socket_address.sin_family = AF_INET;
    socket_address.sin_port = htons(config::port);
    socket_address.sin_addr.s_addr = htonl(INADDR_ANY);

    auto const res = bind(m_socket_fd, reinterpret_cast<sockaddr*>(&socket_address), sizeof(socket_address));

    return res >= 0;
}

auto socket::connect_to_server(std::string const& ip) const noexcept -> bool
{
    sockaddr_in socket_address{};
    std::memset(&socket_address, 0, sizeof(socket_address));
    socket_address.sin_family = AF_INET;
    socket_address.sin_port = htons(config::port);
    inet_pton(AF_INET, ip.c_str(), &socket_address.sin_addr);

    auto const res = ::connect(m_socket_fd, reinterpret_cast<sockaddr*>(&socket_address), sizeof(socket_address));

    return res >= 0;
}

auto socket::listen(int const max_num_conn) const noexcept -> bool
{
    auto const res = ::listen(m_socket_fd, max_num_conn);
    return res >= 0;
}

auto socket::accept() const noexcept -> int
{
    sockaddr_in caddr{};
    int len = sizeof(caddr);
    return ::accept(m_socket_fd, reinterpret_cast<sockaddr*>(&caddr), reinterpret_cast<socklen_t*>(&len));
}

auto socket::wait_for_message() -> std::string
{
    std::string msg{};
    stream_block_t buf{};
    bool stop_reading = false;

    while(!stop_reading) {
        ssize_t num_bytes_read = ::read(m_socket_fd, buf.data(), buf.size() - 1);

        if(num_bytes_read < 0) {
            throw socket_exception{ strerror(errno), m_socket_fd };
        }

        auto* data = reinterpret_cast<char*>(buf.data());
        data[num_bytes_read] = '\0'; // NOLINT
        m_stream_message += std::string{ data };

        auto const sep = m_stream_message.find(config::message_separator);

        if(sep != std::string::npos) {
            msg = m_stream_message.substr(0, sep);
            m_stream_message = m_stream_message.substr(sep + 1);
            stop_reading = true;
        }
    }

    return msg;
}

auto socket::send_message(std::string const& msg) const -> void
{
    auto to_send = msg + config::message_separator;
    bool stop_writing = false;

    while(!stop_writing) {
        ssize_t num_bytes_written = ::write(m_socket_fd, to_send.c_str(), to_send.length());

        if(num_bytes_written < 0) {
            throw socket_exception{ strerror(errno), m_socket_fd };
        }
        if(num_bytes_written == 0) {
            stop_writing = true;
        }
        to_send = to_send.substr(static_cast<std::size_t>(num_bytes_written));
    }
}

auto socket::create_tcp_stream() noexcept -> int
{
    return ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
}

} // namespace net
