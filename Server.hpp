// Server.hpp
#ifndef SERVER_HPP
#define SERVER_HPP

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/filesystem.hpp>
#include <iostream>
#include <thread>

namespace asio = boost::asio;
namespace beast = boost::beast;
namespace http = beast::http;
namespace fs = boost::filesystem;
using tcp = asio::ip::tcp;

class Server {
public:
    explicit Server(unsigned short port);
    void run();

private:
    void accept();
    void handle_session(tcp::socket socket);
    void handle_request(http::request<http::string_body> req, tcp::socket& socket);
    
    asio::io_context io_context_;
    tcp::acceptor acceptor_;
    const std::string SERVE_DIR = "/Users/chansen/WebServer/bin";
};;

#endif // SERVER_HPP