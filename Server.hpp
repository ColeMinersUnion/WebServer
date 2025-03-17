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
    Server(boost::asio::io_context& io_context, short port, const std::string& root_dir, int num_threads);
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
    std::string read_file(const std::string& path, bool& found);
    
    //checks to see if a given file is executable.
    bool isExecutable(const std::string& extension);
    //Forks the process and Executes the file.
    std::string execute(const std::string& path, const std::string& uri, bool& found, int thread_id);

    //* Objects and member variables.
    boost::asio::ip::tcp::acceptor acceptor_;
    std::string root_directory_;
    std::vector<record> current_requests;
    boost::asio::streambuf buffer_; 

    //* Threadpool stuff!
    ThreadPool pool;
};

#endif

