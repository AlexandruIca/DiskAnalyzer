#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace exit_kind {

constexpr int success = EXIT_SUCCESS;
constexpr int failure = EXIT_FAILURE;

} // namespace exit_kind

int main()
{
    sockaddr_in socket_address;
    int res = 0;
    int const socket_fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    if(socket_fd == -1) {
        perror("Cannot create socket!");
        std::exit(exit_kind::failure);
    }

    std::memset(&socket_address, 0, sizeof(socket_address));

    socket_address.sin_family = AF_INET;
    socket_address.sin_port = htons(5678);
    res = inet_pton(AF_INET, "127.0.0.1", &socket_address.sin_addr);

    if(connect(socket_fd, reinterpret_cast<sockaddr*>(&socket_address), sizeof(socket_address)) == -1) {
        perror("Connect failed!");
        close(socket_fd);
        std::exit(exit_kind::failure);
    }

    char const msg[] = "Hello server";
    write(socket_fd, msg, sizeof(msg));

    close(socket_fd);
    return exit_kind::success;
}
