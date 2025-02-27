#include "Server.hpp"

//*Constructor. Initializes the acceptor object to listen on the specified port and the root directory where the files are stored.
//* The acceptor object is used to listen for incoming connections.
Server::Server(boost::asio::io_context& io_context, short port, const std::string& root_dir, int num_threads)
    : acceptor_(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)), root_directory_(root_dir), buffer_(), pool(num_threads) {
        Server::current_requests.resize(num_threads);
    }

//* Starts the server.
void Server::start() {
    Server::current_requests[0].state = IDLE;
    do_accept();
}

//* Uses the acceptor object to asynchronously accept incoming connections.
//* When a connection is accepted, the handle_request function is called.
void Server::do_accept() {
    auto socket = std::make_shared<boost::asio::ip::tcp::socket>(acceptor_.get_executor());
    acceptor_.async_accept(*socket, [this, socket](boost::system::error_code ec) {
        if (!ec) {
            pool.enqueue([this, socket]() mutable {
                handle_request(socket, 0);
            });
        }
        do_accept();
    });
}

//* Handles the incoming request.
void Server::handle_request(std::shared_ptr<boost::asio::ip::tcp::socket> socket, int thread_id) {
    //* Sets the state of the server
    Server::current_requests[thread_id].state = PROCESSING;

    //* Reades the request. '\r\n\r\n' is the end of the request.
    boost::asio::read_until(*socket, buffer_, "\r\n\r\n");

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
    Server::current_requests[thread_id].request = request_line;
    Server::current_requests[thread_id].timestamp = std::time(nullptr);
    Server::current_requests[thread_id].response = response;
    std::cout << "Request: " << Server::current_requests[0].request << std::endl;
    std::cout << "Timestamp: " << Server::current_requests[0].timestamp << std::endl;
    std::cout << "Response: " << Server::current_requests[0].response << std::endl;

    //* Sends the response.
    Server::current_requests[thread_id].state = RESPONDING;
    boost::asio::write(*socket, boost::asio::buffer(response));
    Server::current_requests[thread_id].state = IDLE;

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

