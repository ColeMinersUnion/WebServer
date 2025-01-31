//Libraries I have used before
#include <iostream> //used for print statements
#include <cstring>  //string manipulation

//Libraries I have not used before
// Sys/types is used to define standard data types used in system calls.
#include <sys/types.h> 

// Sys/socket is used to define the socket interface.
// Does most of the heavy lifting in this code. Defines socket, bind, listen, etc.
#include <sys/socket.h>

// Netinet/in is used to define the AF_INET sockaddr_in structure.
#include <netinet/in.h>

// unistd is used for the read, write, and close functions. 
#include <unistd.h>

// Constants
#define PORT 8080 //The port the server is listening on.
#define BUFFER_SIZE 1024 //The size of the buffer used to read the request from the client.

//The port defines where on localhost the server is accessible.

// Executed Code. 
int main() {
    
    // File descriptors for the server and client sockets.
    int server_fd, client_fd; 
    
    //This struct is used to store the IP address and port number of the server.
    struct sockaddr_in address;

    //the length of the address
    int addrlen = sizeof(address);

    //A buffer of buffer size to read the request from the client.
    char buffer[BUFFER_SIZE] = {0};

    // Create socket
    // Assigning the server_fd to the socket function.
    // If the socket function returns 0, then the socket creation failed.
    // The socket function takes three arguments: the address family, the socket type, and the protocol.
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        return -1;
    }

    // Configure server address structure
    // AF_INET specifies IPv4 IP addresses.
    address.sin_family = AF_INET;
    // INADDR_ANY specifies that the server should listen on all network interfaces.
    address.sin_addr.s_addr = INADDR_ANY;
    // htons() converts the port number from host byte order to network byte order.
    // byte order is the order in which bytes are stored in memory.
    // Assures the port number is stored and interpreted in the correct order.
    address.sin_port = htons(PORT);

    // Binds the socket to the port
    // If the bind function returns -1, then the bind failed.
    // The bind function takes three arguments: the server file descriptor, the address of the server, and the length of the address.
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        close(server_fd);
        return -1;
    }

    // Infinite loop to keep the server running after a client disconnects.
    while(true){
        

        // Listen for incoming connections
        // Create a queue of 3 pending connections.
        if (listen(server_fd, 3) < 0) {
            perror("Listen failed");
            close(server_fd);
            return -1;
        }
        
        // Prints the port the server is listening on.
        std::cout << "Server is listening on port " << PORT << "..." << std::endl;

        // Accepts a single connection
        if ((client_fd = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            perror("Accept failed");
            close(server_fd);
            return -1;
        }


        // Read the HTTP request from the client
        // Uses the buffer to store the incoming request.
        read(client_fd, buffer, BUFFER_SIZE);
        std::cout << "Received request:\n" << buffer << std::endl;

        

        // Send HTTP response
        const char *response =
            "HTTP/1.1 200 OK\r\n" // Status code. 200 means the request was successful. (400s for client error, 500s for serer error)
            "Content-Type: text/html\r\n" // The type of content being sent. Tells the browser how to interpret the response.
            "Content-Length: 61\r\n" // The length of the response in bytes. Each character is a byte.
            "\r\n"
            "<!DOCTYPE html><html><body><h1>Hello World</h1></body></html>"; // The response body.
            //Doctype tells the browser what type of document it is.
            //html is the root element of the document, and the body is the content of the document.
            //The h1 element is a heading element, and it formats the enclosed text as a heading.

        // Writes the response to the client. 
        write(client_fd, response, strlen(response));
        std::cout << "Response sent." << std::endl;


        
        // Close the connection to the client.
        close(client_fd);
    }

    // Close the server socket
    close(server_fd);

    return 0;
}
