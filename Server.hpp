// WebServer.hpp
#ifndef SERVER_HPP
#define SERVER_HPP

//* Boost is the framework I used to implement the server.
#include <boost/asio.hpp>

//* Including all of the necessary libraries. 
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <ctime>
#include <vector>
#include <filesystem>

//* HW3
#include "ThreadPool.hpp"

//* Fulfilling a requirement. 
//* Tracks the state of the server.
enum _process_state {
    NEW,
    WAITING,
    PROCESSING,
    RESPONDING,
    FINISHED
};

//* Fulfilling a requirement.
//* Struct object that records relevant information about each request.
struct _record {
    std::string request;
    std::time_t timestamp;
    std::string response;
    pid_t process_id;
    int thread_id;
    enum _process_state state;
};

//* Definig the record struct as record.
typedef struct _record record;

//* I wanted to create a class, becuase trying to adjust the code from last time
//* was more trouble than it was worth. 
class Server {
public:
    //* Constructor. Initializes the acceptor object to listen on the specified port and the root directory where the files are stored.
    Server(boost::asio::io_context& io_context, short port, const std::string& root_dir, int num_threads, const std::string& index_file, const std::string& not_found_file, size_t buffer_size, size_t boost_buf_size);
    //* Starts the server.
    void start();

private:
    
    //* Functions
    //Uses the acceptor object to asynchronously accept incoming connections.
    void do_accept();
    //Handles the incoming request.
    void handle_request(std::shared_ptr<boost::asio::ip::tcp::socket> socket, int thread_id);
    //Figures out how to format the response.
    std::string get_mime_type(const std::string& extension);
    //Reads the file from the bin directory.
    std::string read_file(const std::string& path, int start_pos = 0, int thread_id=0);
    
    //Checks to see if the file exists.
    bool fileFound(const std::string& path);

    //checks to see if a given file is executable.
    bool isExecutable(const std::string& extension);
    //Forks the process and Executes the file.
    std::string execute(const std::string& path, const std::string& uri, int thread_id);

    //* Objects and member variables.
    boost::asio::ip::tcp::acceptor acceptor_;
    std::string root_directory_;
    std::vector<record> current_requests;
    boost::asio::streambuf buffer_; 

    //Will be configured later. 
    const std::string index_file;
    const std::string not_found_file;
    

    //Fixing something I should have done a while ago I suppose
    size_t rd_buf_size;
    size_t boost_buf_size;

    std::vector<boost::asio::mutable_buffer> network_buffers;
    std::vector<char*> read_buffers;
    
    int num_threads;

    //* Threadpool stuff!
    ThreadPool pool;
};

#endif

