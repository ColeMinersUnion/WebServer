#include "Server.hpp"

//*Constructor. Initializes the acceptor object to listen on the specified port and the root directory where the files are stored.
//* The acceptor object is used to listen for incoming connections.
Server::Server(boost::asio::io_context& io_context, short port, const std::string& root_dir, int num_threads, const std::string& index_file, const std::string& not_found_file, size_t buffer_size, size_t boost_buf_size)
    : acceptor_(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)), root_directory_(root_dir), buffer_(), pool(num_threads), index_file(index_file), not_found_file(not_found_file), rd_buf_size(buffer_size), boost_buf_size(boost_buf_size) {
        Server::current_requests.resize(num_threads);
    }

//* Starts the server.
void Server::start() {

    //set maximum size of buffer_ to boost_buf_size


    for (int i = 0; i < Server::current_requests.size(); i++) {
        Server::current_requests[i].state = WAITING;;
        Server::current_requests[i].thread_id = i;
    }
    do_accept();
}

//* Uses the acceptor object to asynchronously accept incoming connections.
//* When a connection is accepted, the handle_request function is called.
void Server::do_accept() {
    auto socket = std::make_shared<boost::asio::ip::tcp::socket>(acceptor_.get_executor());
    acceptor_.async_accept(*socket, [this, socket](boost::system::error_code ec) {
        if (!ec) {
            pool.enqueue([this, socket](int thread_id) mutable {
                handle_request(socket, thread_id); // Pass thread_id
            });
        }
        do_accept();
    });
}

//* Handles the incoming request.
void Server::handle_request(std::shared_ptr<boost::asio::ip::tcp::socket> socket, int thread_id) {
    //* Sets the state of the server
    Server::current_requests[thread_id].state = PROCESSING;

    //* Reads the request. '\r\n\r\n' is the end of the request.
    boost::system::error_code ec;
    size_t bytes_transferred = boost::asio::read_until(*socket, buffer_, "\r\n\r\n", ec);
    //size_t bytes_transferred = boost::asio::read(*socket, buffer_, boost::asio::transfer_at_least(1), ec);



    if (ec == boost::asio::error::eof) {
        std::cerr << "Client closed the connection prematurely (EOF encountered)" << std::endl;
        Server::current_requests[thread_id].state = FINISHED;
        return;
    } else if (ec) {
        std::cerr << "Error reading request: " << ec.message() << std::endl;
        Server::current_requests[thread_id].state = FINISHED;
        return;
    }

    //* Parses the request.
    std::istream request_stream(&buffer_);
    std::string request_line;
    std::getline(request_stream, request_line);
    std::istringstream request_line_stream(request_line);

    //* Breaks down the request into its components.
    std::string method, uri, version;
    request_line_stream >> method >> uri >> version;

    //* If the uri is empty, set it to index.html.
    if (uri == "/") uri = Server::index_file;

    //* File navigation purposes
    std::string file_path = root_directory_ + uri;
    bool file_found;

    //Request body needs to be statically allocated at the beginning of the program
    std::string request_body;
    //* If the file is executable, execute it.
    file_found = fileFound(file_path);
    if (!file_found){
        std::cout << "File not found. Using 404.html" << std::endl;
        file_path = root_directory_ + Server::not_found_file;
        
        size_t file_size = std::filesystem::file_size(file_path);
        for(int i = 0; i < file_size/rd_buf_size + 1; i++){
            request_body += read_file(file_path, i*rd_buf_size);
        }

    }
    else if (isExecutable(uri)) {
        std::cout << "Executable file found" << std::endl;
        request_body = execute(file_path, uri, thread_id);
    } else {
        //* Otherwise read in the file
        size_t file_size = std::filesystem::file_size(file_path);
        for(int i = 0; i < file_size/rd_buf_size+1; i++){
            request_body += read_file(file_path, i*rd_buf_size);
        }


    }
    std::cout << "\nRequest body: " << request_body << "\n" << std::endl;
    //std::cout << get_mime_type(file_path) << std::endl;
    //* Streaming the response to the client.
    std::ostringstream response_stream;
    response_stream << "HTTP/1.1 200 OK\r\n";
    response_stream << "Content-Length: " << request_body.size() << "\r\n";
    response_stream << "Content-Type: " << get_mime_type(file_path) << "\r\n\r\n";
    //*the appropriate response
    response_stream << request_body;
    std::cout <<"\nWe got this far\n" << std::endl;
    //* Converts to a response.
    std::string response = response_stream.str();

    //* Records the request in the control block
    Server::current_requests[thread_id].request = request_line;
    Server::current_requests[thread_id].timestamp = std::time(nullptr);
    Server::current_requests[thread_id].response = response;
    //* Printing parts of the request for validation purposes.
    std::cout << "Request: " << Server::current_requests[thread_id].request << std::endl;
    std::cout << "Timestamp: " << Server::current_requests[thread_id].timestamp << std::endl;
    std::cout << "Response: " << Server::current_requests[thread_id].response << std::endl;

    //* Sends the response.
    Server::current_requests[thread_id].state = RESPONDING;
    std::string response_chunk;
    for(int i = 0; i < response.size()/boost_buf_size+1; i++){
        response_chunk = response.substr(i*boost_buf_size, (i+1)*boost_buf_size);
        boost::asio::write(*socket, boost::asio::buffer(response_chunk, boost_buf_size));
        std::cout << "Sending chunk: " << response_chunk << std::endl;
    }
    //boost::asio::write(*socket, boost::asio::buffer(response, boost_buf_size));
    Server::current_requests[thread_id].state = FINISHED;

    buffer_.consume(buffer_.size());
    //* Clear the buffer for the next request
}

//* Reading files from the directory. 
std::string Server::read_file(const std::string& path, int start_pos) {
    //* Does the file exist?
    std::ifstream file(path, std::ios::binary);
    //* Stream the contents back to the handler

    //Setting the size of the file read buffer here!
    // get pointer to associated buffer object
    std::filebuf* pbuf = file.rdbuf();
    pbuf->pubseekpos(start_pos);


    // allocate memory to contain file data
    char* buffer=new char[rd_buf_size];

    // get file data
    pbuf->sgetn (buffer, rd_buf_size);

    return std::string(buffer);
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
    if (extension.ends_with(".exe")) return "text/plain";
    return "application/octet-stream";
}

bool Server::isExecutable(const std::string& extension){
    //* Is the executable file an object file? (Can I execute it)
    if (extension.ends_with(".o")) return true;
    if (extension.ends_with(".exe")) return true;
    return false;
}

bool Server::fileFound(const std::string &path){
    if (access(path.c_str(), F_OK) == -1) {
        std::cerr << "File not found" << std::endl;
        std::cerr << "Path: " << path << std::endl;
        return false;
    }
    return true;
}


/*
  This function forks the process.
  The forked process will execute the file at the path.
  The parent process will wait for the child process to finish executing.
  The function will return the output of the child process. 
*/
std::string Server::execute(const std::string& path, const std::string& uri, int thread_id){
    // Debugged with the help of Mr. GPT
    // Check if file exists using access
    
    //* The current working directory is in ./build
    //* I need to access the bin directory to execute the file
    std::string exe_path_str = "../bin" + uri;
    std::cout << "Executing: " << exe_path_str << std::endl;

    //Creating a pipe to return execvp errors to the uers.
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");

        return "Failed to create pipe";
    }

    //Forking the 
    pid_t pid = fork();
    Server::current_requests[thread_id].process_id = pid;

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
        // Read from pipe until EOF and closing the pipe.
        while ((count = read(pipefd[0], buffer, sizeof(buffer) - 1)) > 0) {
            buffer[count] = '\0';
            output += buffer;
        }
        close(pipefd[0]);

        int status;
        waitpid(pid, &status, 0);
        // Check if process exited normally or by signal
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


//* Completion condition for boost::asio::read