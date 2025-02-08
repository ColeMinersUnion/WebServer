// main.cpp
#include "Server.hpp"
#include <boost/asio.hpp>

int main() {
    try {
        boost::asio::io_context io_context;
        //!Note to self: Since I'm using CMAKE, I must use absolute paths for file io.
        Server server(io_context, 8000, "/Users/chansen/WebServer/bin");
        server.start();
        io_context.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return 0;
}