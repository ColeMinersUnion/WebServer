#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <boost/asio.hpp>
#include <boost/socket.hpp>
#include <boost/filesystem.hpp>
#include <string>

class Server {
public:
    // Constructors
    Server();
    ~Server();

    // Singleton instantiation (placed in another .cpp file)
    static Server& getServer();

    // Methods
    void start();
    void stop();
    bool isRunning() const;
    void sendMessage(const std::string& message);
    void listen(const std::string& port);

private:
    // Private members go here
    bool running;
    // Other necessary member variables
};

#endif