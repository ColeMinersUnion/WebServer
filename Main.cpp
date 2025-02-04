// main.cpp
#include "Server.hpp"
#include <boost/asio.hpp>

int main() {
    try {
        boost::asio::io_context io_context;
        Server server(io_context, 8000, "./bin");
        server.start();
        io_context.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return 0;
}