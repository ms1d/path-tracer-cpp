#include <asio.hpp>
#include <iostream>

using asio::ip::udp;

int main() {
    asio::io_context io;

    udp::socket socket(io, udp::endpoint(udp::v4(), 9000));

    char data[1024];
    udp::endpoint sender;

    socket.async_receive_from(
        asio::buffer(data),
        sender,
        [&](const std::error_code& ec, std::size_t bytes) {
            if (!ec) {
                std::cout << "Received: "
                          << std::string(data, bytes) << "\n";
            }
        }
    );

    io.run();
}
