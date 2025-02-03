// Server.cpp
#include "Server.hpp"

Server::Server(unsigned short port) : acceptor_(io_context_, {tcp::v4(), port}) {}

void Server::run() {
    accept();
    io_context_.run();
}

void Server::accept() {
    acceptor_.async_accept([this](boost::system::error_code ec, tcp::socket socket) {
        if (!ec) {
            std::thread(&Server::handle_session, this, std::move(socket)).detach();
        }
        accept();
    });
}

void Server::handle_session(tcp::socket socket) {
    try {
        beast::flat_buffer buffer;
        http::request<http::string_body> req;
        http::read(socket, buffer, req);
        handle_request(std::move(req), socket);
    } catch (const std::exception& e) {
        std::cerr << "Session error: " << e.what() << std::endl;
    }
}

void Server::handle_request(http::request<http::string_body> req, tcp::socket& socket) {
    http::response<http::file_body> res;
    std::string target = req.target();
    if (target == "/") target = "/index.html";

    fs::path file_path = fs::path(SERVE_DIR) / target.substr(1);
    if (!fs::exists(file_path) || fs::is_directory(file_path)) {
        res.result(http::status::not_found);
        res.set(http::field::content_type, "text/plain");
        res.set(http::field::body, "File not found");
        res.prepare_payload();
    } else {
        beast::error_code ec;
        http::file_body::value_type body;
        body.open(file_path.string().c_str(), beast::file_mode::scan, ec);
        if (ec) {
            res.result(http::status::internal_server_error);
            res.set(http::field::content_type, "text/plain");
            res.set(http::field::body, "Failed to open file");
            res.prepare_payload();
        } else {
            res.result(http::status::ok);
            res.set(http::field::content_type, "application/octet-stream");
            res.body() = std::move(body);
        }
    }
    http::write(socket, res);
}