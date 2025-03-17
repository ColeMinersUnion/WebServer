#include "Server.hpp"

//*Constructor. Initializes the acceptor object to listen on the specified port and the root directory where the files are stored.
//* The acceptor object is used to listen for incoming connections.
Server::Server(boost::asio::io_context& io_context, short port, const std::string& root_dir, int num_threads)
    : acceptor_(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)), root_directory_(root_dir), buffer_(), pool(num_threads) {
        Server::current_requests.resize(num_threads);
    }

//* Starts the server.
void Server::start() {
    Server::current_requests[0].state = WAITING;
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
    std::string request_body;;
    //* If the file is executable, execute it.
    if (isExecutable(uri)) {
        std::cout << "Executable file found" << std::endl;
        request_body = execute(file_path, uri, file_found);
    } else {
        request_body = read_file(file_path, file_found);

    }


    std::ostringstream response_stream;
    if (file_found) {
        //* Responds by sending the requested file.
        response_stream << "HTTP/1.1 200 OK\r\n";
        response_stream << "Content-Length: " << request_body.size() << "\r\n";
        response_stream << "Content-Type: " << get_mime_type(file_path) << "\r\n\r\n";
        //*the appropriate response
        response_stream << request_body;
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
    Server::current_requests[thread_id].state = FINISHED;

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
    //Executable files will return text/plain to return any errors and what not

    if (extension.ends_with(".o")) return "text/plain";
    return "application/octet-stream";
}

bool Server::isExecutable(const std::string& extension){
    //if (extension.ends_with(".exe")) return true;
    //if (extension.ends_with(".sh")) return true;
    //if (extension.ends_with(".bat")) return true;
    if (extension.ends_with(".o")) return true;
    return false;
}

/*
  This function forks the process.
  The forked process will execute the file at the path.
  The parent process will wait for the child process to finish executing.
  The function will return the output of the child process. 
*/
std::string Server::execute(const std::string& path, const std::string& uri, bool& found){
    // Debugged with the help of Mr. GPT
    // Check if file exists using access()
    if (access(path.c_str(), F_OK) == -1) {
        std::cerr << "File not found" << std::endl;
        found = false;
        return "";
    }
    found = true;

    std::string exe_path_str = "../bin" + uri;
    std::cout << "Executing: " << exe_path_str << std::endl;

    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");

        return "Failed to create pipe";
    }

    pid_t pid = fork();
    Server::current_requests[0].process_id = pid;

    if (pid == 0) {
        // Child process
        close(pipefd[0]); // Close read end of pipe
        dup2(pipefd[1], STDOUT_FILENO); // Redirect stdout to pipe
        dup2(pipefd[1], STDERR_FILENO); // Redirect stderr to pipe
        close(pipefd[1]);

        // Execute the file
        char* args[] = {const_cast<char*>(exe_path_str.c_str()), nullptr};
        execvp(args[0], args);

        // execvp only returns if it fails
        perror("execvp");
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        // Parent process
        close(pipefd[1]); // Close write end of pipe

        // Capture output
        std::string output;
        char buffer[256];
        ssize_t count;
        while ((count = read(pipefd[0], buffer, sizeof(buffer) - 1)) > 0) {
            buffer[count] = '\0';
            output += buffer;
        }
        close(pipefd[0]);

        int status;
        waitpid(pid, &status, 0);

        if (WIFEXITED(status)) {
            std::cout << "Process finished with status: " << WEXITSTATUS(status) << std::endl;
        } else if (WIFSIGNALED(status)) {
            std::cerr << "Process terminated by signal: " << WTERMSIG(status) << std::endl;
        }

        return output;
    } else {
        // Fork failed
        perror("fork");
        return "Fork failed";
    }
}