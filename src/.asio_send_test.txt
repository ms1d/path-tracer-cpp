#include <asio.hpp>

using asio::ip::udp;

int main() {
    asio::io_context io;

    udp::socket socket(io);
    socket.open(udp::v4());

    udp::endpoint target(
        asio::ip::make_address("127.0.0.1"), 9000
    );

    std::string msg = "hello";

    socket.send_to(asio::buffer(msg), target);
}
