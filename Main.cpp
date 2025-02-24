// main.cpp
#include "Server.hpp"
#include <boost/asio.hpp>


int main() {
    try {
        // Create an io_context object, describes the I/O context used by the server.
        boost::asio::io_context io_context;
        //!Note to self: Since I'm using CMAKE, I must use absolute paths for file io.
        //* Instantiates the server object.
        //* IO context, port, root directory, and number of threads.
        Server server(io_context, 8000, "/Users/chansen/WebServer/bin", 3);
        //* Starts the server
        server.start();
        //* Runs the io_context object.
        io_context.run();
    } catch (const std::exception& e) {
        //* Used for debugging.
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return 0;
}