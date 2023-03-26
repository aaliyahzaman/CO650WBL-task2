// This is a simple client program that connects to a server and sends
// data to it. The server will then send a response back to the client.
// The client will then print the response to the console.

// below are the libraries that are used in this program
#include <iostream>
// #include <string> is used to store the string
#include <string>
// #include <cstring> is used to store the string
#include <cstring>
// #include <sys/socket.h> is used to create a socket
#include <sys/socket.h>
// #include <netinet/in.h> is used to store the address of the server
#include <arpa/inet.h>
// #include <unistd.h> is used to close the socket
#include <unistd.h>

// using namespace std is used to avoid writing std::cout and std::endl etc
using namespace std;

// SERVER_IP is the IP address of the server its a global variable
#define SERVER_IP "127.0.0.1" // IP address of the server
// SERVER_PORT is the port number of the server its a global variable
#define SERVER_PORT 8888 // Port number of the server

// Data is a class that is used to store the data that is sent to the server
class Data {
public:
    // msg is a string that is used to store the message that is sent to the server
    std::string msg = "";
};

int main() {
    // Create a socket by calling the socket() function and store the result in the sock variable
    int sock = socket(AF_INET, SOCK_STREAM, 0); // Create a TCP socket
    if (sock == -1) {
        // If the socket is not created then print the error message
        std::cerr << "Failed to create socket\n";
        return 1;
    }

    // Specify the address and port of the server to connect to in the server_addr variable
    sockaddr_in server_addr;
    // Clear the server_addr variable by setting all the bytes to 0
    // This is done to avoid any garbage values
    memset(&server_addr, 0, sizeof(server_addr));
    // Set the address family to AF_INET which is the IPv4 address family so we can connect to the loopback address 127.0.0.1
    server_addr.sin_family = AF_INET;
    // Set the port number of the server to connect to by converting the port number to network byte order using htons()
    server_addr.sin_port = htons(SERVER_PORT);
    // Set the IP address of the server to connect to by converting the IP address to network byte order using inet_pton()
    // inet_pton() is used to convert the IP address from a string to a binary value
    // inet_pton() returns 1 if the conversion was successful
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address or address not supported\n";
        return 1;
    }

    // Connect to the server by calling the connect() function of the socket and passing the server address and port
    // connect() returns 0 if the connection was successful
    // connect() returns -1 if the connection was unsuccessful
    if (connect(sock, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Failed to connect to server\n";
        return 1;
    }

    while (true) {
        // Send and receive data in a loop until the user types 'quit'
        Data dataSend;
        // Get the user input and store it in the msg variable of the dataSend object of the Data class
        std::cout << "Enter message for the server (type 'quit' to exit): ";
        // Prompt the user for input and store it in the msg variable of the dataSend object of the Data class
        std::getline(std::cin, dataSend.msg);

        // Send the data to the server by calling the send() function of the socket
        // send() returns the number of bytes sent if the data was sent successfully
        // send() returns -1 if the data was not sent successfully
        if (send(sock, &dataSend, sizeof(dataSend), 0) < 0) {
            std::cerr << "Failed to send data\n";
            return 1;
        }

        // Check if the user wants to quit the program
        // If the user types 'quit' then break out of the loop
        if (dataSend.msg == "quit") {
            std::cout << "Closing connection and exiting...\n";
            break;
        }


        Data dataRecv;
        // Receive a response from the server by calling the recv() function of the socket
        // recv() returns the number of bytes received if the data was received successfully
        int recv_len = recv(sock, &dataRecv, sizeof(dataRecv), 0);

        // Check if the data was received successfully
        if (recv_len < 0) {
            // If the data was not received successfully then print the error message and break out of the loop
            std::cerr << "Failed to receive data\n";
            break;
        }

        // Check if the server is shutting down or if the user typed 'QUIT'
        // If the server is shutting down or if the user typed 'SHUTDOWN' then break out of the loop
        if (std::string(dataRecv.msg) == "SHUTDOWN" || std::string(dataRecv.msg) == "QUIT") {
            std::cout << "Server is shutting down\n";
            break;
        }
        // Print the response from the server to the console
        // The response is stored in the msg variable of the dataRecv object of the Data class
        // The response is printed using the std::cout object of the std::cout class
        std::cout << "Server response: " << dataRecv.msg << std::endl;
    }

    close(sock); // Close the socket when done sending and receiving data
    return 0;
}
