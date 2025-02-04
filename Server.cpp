#include "Server.hpp"

Server::Server(boost::asio::io_context& io_context, short port, const std::string& root_dir)
    : acceptor_(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)), root_directory_(root_dir) {}

void Server::start() {
    do_accept();
}

void Server::do_accept() {
    acceptor_.async_accept([this](boost::system::error_code ec, boost::asio::ip::tcp::socket socket) {
        if (!ec) {
            handle_request(std::move(socket));
        }
        do_accept();
    });
}

void Server::handle_request(boost::asio::ip::tcp::socket socket) {
    boost::asio::streambuf buffer;
    boost::asio::read_until(socket, buffer, "\r\n\r\n");
    std::istream request_stream(&buffer);
    std::string request_line;
    std::getline(request_stream, request_line);

    std::istringstream request_line_stream(request_line);
    std::string method, uri, version;
    request_line_stream >> method >> uri >> version;

    if (uri == "/") uri = "/index.html";
    std::string file_path = root_directory_ + uri;
    bool file_found;
    std::string file_content = read_file(file_path, file_found);

    std::ostringstream response_stream;
    if (file_found) {
        response_stream << "HTTP/1.1 200 OK\r\n";
        response_stream << "Content-Length: " << file_content.size() << "\r\n";
        response_stream << "Content-Type: " << get_mime_type(file_path) << "\r\n\r\n";
        response_stream << file_content;
    } else {
        response_stream << "HTTP/1.1 404 Not Found\r\n\r\n";
    }

    std::string response = response_stream.str();
    boost::asio::write(socket, boost::asio::buffer(response));
}

std::string Server::read_file(const std::string& path, bool& found) {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        found = false;
        return "";
    }
    found = true;
    std::ostringstream contents;
    contents << file.rdbuf();
    return contents.str();
}

std::string Server::get_mime_type(const std::string& extension) {
    if (extension.ends_with(".html")) return "text/html";
    if (extension.ends_with(".txt")) return "text/plain";
    if (extension.ends_with(".jpg")) return "image/jpeg";
    if (extension.ends_with(".png")) return "image/png";
    if (extension.ends_with(".css")) return "text/css";
    return "application/octet-stream";
}

