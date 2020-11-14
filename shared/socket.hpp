#ifndef SOCKET_HPP
#define SOCKET_HPP
#pragma once

#include <array>
#include <stdexcept>
#include <string>

namespace net {

class socket_exception : public std::exception
{
private:
    std::string m_msg;

public:
    socket_exception() = default;
    socket_exception(socket_exception const&) = default;
    socket_exception(socket_exception&&) noexcept = default;
    ~socket_exception() noexcept override = default;

    explicit socket_exception(std::string msg, int fd);

    auto operator=(socket_exception const&) -> socket_exception& = default;
    auto operator=(socket_exception&&) noexcept -> socket_exception& = default;

    [[nodiscard]] auto what() const noexcept -> char const* override;
};

using stream_block_t = std::array<std::byte, 1024>;

class socket
{
private:
    std::string m_stream_message{};
    int m_socket_fd = 0;

public:
    socket() = delete;
    socket(socket const&) = delete;
    socket(socket&&) noexcept = default;
    ~socket() noexcept;

    explicit socket(int fd);

    auto operator=(socket const&) -> socket& = delete;
    auto operator=(socket&&) noexcept -> socket& = default;

    [[nodiscard]] auto get_fd() const noexcept -> int;
    [[nodiscard]] auto ok() const noexcept -> bool;
    [[nodiscard]] auto create_server() const noexcept -> bool;
    [[nodiscard]] auto connect_to_server(std::string const& ip = "127.0.0.1") const noexcept -> bool;
    [[nodiscard]] auto listen(int max_num_conn) const noexcept -> bool;
    [[nodiscard]] auto accept() const noexcept -> int;
    [[nodiscard]] auto wait_for_message() -> std::string;
    auto send_message(std::string const& msg) const -> void;

    [[nodiscard]] static auto create_tcp_stream() noexcept -> int;
};

} // namespace net

#endif // !SOCKET_HPP
