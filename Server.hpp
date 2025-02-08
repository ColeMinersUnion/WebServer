
// WebServer.hpp
#ifndef SERVER_HPP
#define SERVER_HPP

#include <boost/asio.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

class Server {
public:
    Server(boost::asio::io_context& io_context, short port, const std::string& root_dir);
    void start();

private:
    void do_accept();
    void handle_request(boost::asio::ip::tcp::socket socket);
    std::string generate_response(const std::string& request);
    std::string get_mime_type(const std::string& extension);
    std::string read_file(const std::string& path, bool& found);


    boost::asio::ip::tcp::acceptor acceptor_;
    std::string root_directory_;
};

#endif // WEBSERVER_HPP

