#include "Server.hpp"

// Singleton instance of Server
static Server& serverInstance = Server::getServer();

// Usage example
int main() {
    // Use the singleton instance here
    serverInstance.listen("8080");
    // Other usage code
    return 0;
}
