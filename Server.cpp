#include "Server.hpp"

//*Constructor. Initializes the acceptor object to listen on the specified port and the root directory where the files are stored.
//* The acceptor object is used to listen for incoming connections.
Server::Server(boost::asio::io_context& io_context, short port, const std::string& root_dir)
    : acceptor_(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)), root_directory_(root_dir), buffer_() {}

//* Starts the server.
void Server::start() {
    Server::current_request.state = IDLE;
    do_accept();
}

//* Uses the acceptor object to asynchronously accept incoming connections.
//* When a connection is accepted, the handle_request function is called.
void Server::do_accept() {
    acceptor_.async_accept([this](boost::system::error_code ec, boost::asio::ip::tcp::socket socket) {
        if (!ec) {
            handle_request(std::move(socket));
        }
        do_accept();
    });
}

//* Handles the incoming request.
void Server::handle_request(boost::asio::ip::tcp::socket socket) {
    //* Sets the state of the server
    Server::current_request.state = PROCESSING;

    //* Reades the request. '\r\n\r\n' is the end of the request.
    boost::asio::read_until(socket, buffer_, "\r\n\r\n");

    //* Parses the request.
    std::istream request_stream(&buffer_);
    std::string request_line;
    std::getline(request_stream, request_line);
    std::istringstream request_line_stream(request_line);

    //* Breaks down the request into its components.
    std::string method, uri, version;
    request_line_stream >> method >> uri >> version;

    //* If the uri is empty, set it to index.html.
    if (uri == "/") uri = "/index.html";

    //* File navigation purposes
    std::string file_path = root_directory_ + uri;
    bool file_found;
    std::string file_content = read_file(file_path, file_found);

    std::ostringstream response_stream;
    if (file_found) {
        //* Responds by sending the requested file.
        response_stream << "HTTP/1.1 200 OK\r\n";
        response_stream << "Content-Length: " << file_content.size() << "\r\n";
        response_stream << "Content-Type: " << get_mime_type(file_path) << "\r\n\r\n";
        response_stream << file_content;
    } else {
        //* Responds with a 404 error.
        response_stream << "HTTP/1.1 404 Not Found\r\n\r\n";
    }

    //* Converts to a response.
    std::string response = response_stream.str();

    //* Records the request. 
    Server::current_request.request = request_line;
    Server::current_request.timestamp = std::time(nullptr);
    Server::current_request.response = response;
    std::cout << "Request: " << Server::current_request.request << std::endl;
    std::cout << "Timestamp: " << Server::current_request.timestamp << std::endl;
    std::cout << "Response: " << Server::current_request.response << std::endl;

    //* Sends the response.
    Server::current_request.state = RESPONDING;
    boost::asio::write(socket, boost::asio::buffer(response));
    Server::current_request.state = IDLE;

    buffer_.consume(buffer_.size());
    //* Clear the buffer for the next request
}

//* Reading files from the directory. 
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
    //* Telling the client how to interpret the response file 
    if (extension.ends_with(".html")) return "text/html";
    if (extension.ends_with(".txt")) return "text/plain";
    if (extension.ends_with(".jpg")) return "image/jpeg";
    if (extension.ends_with(".png")) return "image/png";
    if (extension.ends_with(".css")) return "text/css";
    return "application/octet-stream";
}

