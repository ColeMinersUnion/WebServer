// WebServer.hpp
#ifndef SERVER_HPP
#define SERVER_HPP

#include <boost/asio.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <ctime>

enum _process_state {
    IDLE,
    PROCESSING,
    RESPONDING
};


struct _record {
    std::string request;
    std::time_t timestamp;
    std::string response;
    enum _process_state state;
};

typedef struct _record record;

class Server {
public:
    Server(boost::asio::io_context& io_context, short port, const std::string& root_dir);
    void start();

private:
    void do_accept();
    void handle_request(boost::asio::ip::tcp::socket socket);
    std::string get_mime_type(const std::string& extension);
    std::string read_file(const std::string& path, bool& found);
    
    boost::asio::ip::tcp::acceptor acceptor_;
    std::string root_directory_;
    record current_request;
    boost::asio::streambuf buffer_; // Add buffer member variable
};

#endif // WEBSERVER_HPP

