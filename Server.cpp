//Libraries I have used before
#include <iostream> //used for print statements
#include <cstring>  //string manipulation

//Libraries I have not used before
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

// Constants
#define PORT 8080
#define BUFFER_SIZE 1024

//The port defines where on localhost the server is accessible.

// Executed Code. 
int main() {
    int server_fd, client_fd; 
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        return -1;
    }

    // Configure server address structure
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind the socket to the port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        close(server_fd);
        return -1;
    }

    while(true){
        

        // Listen for incoming connections
        if (listen(server_fd, 3) < 0) {
            perror("Listen failed");
            close(server_fd);
            return -1;
        }

        std::cout << "Server is listening on port " << PORT << "..." << std::endl;

        // Accept a single connection (non-multithreaded)
        if ((client_fd = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            perror("Accept failed");
            close(server_fd);
            return -1;
        }


        // Read the HTTP request from the client
        read(client_fd, buffer, BUFFER_SIZE);
        std::cout << "Received request:\n" << buffer << std::endl;

        

        // Send HTTP response
        const char *response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: 61\r\n"
            "\r\n"
            "<!DOCTYPE html><html><body><h1>Hello World</h1></body></html>";

        write(client_fd, response, strlen(response));
        std::cout << "Response sent." << std::endl;


        
        // Close the connection
        close(client_fd);
    }
    // TODO: Implement a loop to accept multiple connections, and only close on failure or signal
    close(server_fd);

    return 0;
}
