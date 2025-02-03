// main.cpp
#include "Server.hpp"

int main() {
    try {
        unsigned short port = 8000;
        Server server(port);
        std::cout << "Server running on port " << port << "..." << std::endl;
        server.run();
    } catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << std::endl;
    }
    return 0;
}