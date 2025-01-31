#include "Server.hpp"

inline Server& Server::getServer() {
    static Server instance;
    return instance;
}

void Server::start() {
    // Start server logic here
}
void Server::stop() {
    // Stop server logic here
}
bool Server::isRunning() const {
    return running;
}
void sendMessage(const std::string& message) {
    // Send message logic here
}
void listen(const std::string& port) {
    // Listen on specified port here
}

// Private members are declared in the header file