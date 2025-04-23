// main.cpp
#include "Server.hpp"
#include <boost/asio.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map>



int main() {

    //* Load in configuration file
    
    std::ifstream config = std::ifstream("../WebServer.cfg");
    if (!config) {
        std::cerr << "Error opening configuration file." << std::endl;
        return 1;
    }
    std::unordered_map<std::string, std::string> config_map;
    std::string configuration;
    while(getline(config, configuration)){
        std::istringstream iss(configuration);
        std::string key, value;
        if (std::getline(iss, key, '=') && std::getline(iss, value)) {
            config_map[key] = value;
        }
    }

    
    // std::cout << config_map["ROOT"] << std::endl;
    // std::cout << config_map["INDEX"] << std::endl;
    // std::cout << config_map["FILE_NOT_FOUND"] << std::endl;
    // std::cout << std::stoi(config_map["NUM_THREADS"]) << std::endl;
    // std::cout << config_map["PORT"] << std::endl; 
            

    
    


    try {
        // Create an io_context object, describes the I/O context used by the server.
        boost::asio::io_context io_context;
        //!Note to self: Since I'm using CMAKE, I must use absolute paths for file io.
        //* Instantiates the server object.
        //* IO context, port, root directory, and number of threads.
        Server server(
            io_context, 
            std::stoi(config_map["PORT"]), 
            config_map["ROOT"], 
            std::stoi(config_map["NUM_THREADS"]), 
            config_map["INDEX"], 
            config_map["FILE_NOT_FOUND"],
            std::stoi(config_map["FILE_BUFFER_SIZE"])
        );
        //* Starts the server
        server.start();
        //* Runs the io_context object.
        io_context.run();
    } catch (const std::exception& e) {
        //* Used for debugging.
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}