#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <functional>

// SERVER_PORT is the port number of the server its a global variable
// we are on server side so we are using port 8888 which the client will connect to
#define SERVER_PORT 8888

// Data is a class that is used to store the data that is sent to the server
class Data {
public:
    std::string msg = "";
};

void client_handler(int& client_sock) {
        // lets create a thread to handle the input from the client and send it to the server
        // we will use a lambda function to create the thread and pass the client socket as a reference to the thread
        // the thread will run in the background and will not block the main thread
        // the thread will run until the user enters "SHUTDOWN" or "QUIT"
        // the thread will send the input to the server and then wait for the server to send a response
        // the thread will then print the response to the console

        std::thread input_thread([&client_sock]() {
            while (true) {
                // Create a Data object to store the data to send to the server
                Data dataSend;
                // Read input from the server side and send it to the client by putting it in the Data object
                std::cout << "Enter a message for the client: ";
                std::getline(std::cin, dataSend.msg);

                // Send the input to the client by calling the send() function of the socket
                // send() returns the number of bytes sent if the send was successful
                // send() returns -1 if the send was unsuccessful
                if (send(client_sock, &dataSend, sizeof(dataSend), 0) < 0) {
                    std::cerr << "Failed to send data to client\n";
                    break;
                }
            }
        }
        );

        // lets create a thread to handle the output from the server and print it to the console
        // we will use a lambda function to create the thread and pass the client socket as a reference to the thread
        // the thread will run in the background and will not block the main thread
        // the thread will run until the user enters "SHUTDOWN" or "QUIT"
        // the thread will wait for the server to send a response
        // the thread will then print the response to the console
        std::thread output_thread([&client_sock]() {
            while (true) {
                // Create a Data object to store the data received from the server
                Data dataRecv;

                // Receive data from the client by calling the recv() function of the socket
                int recv_len = recv(client_sock, &dataRecv, sizeof(dataRecv), 0);
                // recv() returns the number of bytes received if the receive was successful
                // recv() returns 0 if the client disconnected
                if (recv_len < 0) {
                    std::cerr << "Failed to receive data\n";
                    break;
                }

                // Check if the name is "SHUTDOWN" or "QUIT" to exit the loop and close the socket
                if (std::string(dataRecv.msg) == "SHUTDOWN" || std::string(dataRecv.msg) == "QUIT") {
                    std::cout << "Shutting down handler thread\n";
                    break;
                }
                // Print the received message to the console if the message is not "SHUTDOWN" or "QUIT"
                std::cout << "Received Message=" << dataRecv.msg << ".\n" << std::endl;

                // Send a response back to the client by calling the send() function of the socket
                const char* response = "\n Data received";
                // send() returns the number of bytes sent if the send was successful 
                // send() returns -1 if the send was unsuccessful
                // send() returns 0 if the client disconnected
                if (send(client_sock, response, strlen(response), 0) < 0) {
                    std::cerr << "Failed to send response\n";
                    break;
                }
            }
            }
        );

    // Join the input thread before exiting the function
    // This will block the main thread until the input thread is done
    // This will ensure that the client socket is closed before the main thread exits
    // This will also ensure that the client socket is closed before the main thread creates a new client socket
    // If the client socket is not closed before the main thread exits, the client socket will be closed by the OS
    input_thread.join();
    output_thread.join();

    // Close the client socket when done with the client
    close(client_sock); // Close the client socket when done
}


int main() {
    // Create a socket for the server to listen for client connections
    int server_sock = socket(AF_INET, SOCK_STREAM, 0); // Create a TCP socket

    // Check if the socket was created successfully by checking if the socket descriptor is -1
    if (server_sock == -1) {
        std::cerr << "Failed to create socket\n";
        return 1;
    }

    // Create a sockaddr_in object to store the address of the server
    // The sockaddr_in object is used to store the address of the server
    // The sockaddr_in object is used to bind the socket to the server address
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    // This will allow the server to listen for connections on any IP address
    // This will also allow the server to send and receive data on any IP address
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // This will allow the server to listen for connections on the port SERVER_PORT
    // This will also allow the server to send and receive data on the port SERVER_PORT
    server_addr.sin_port = htons(SERVER_PORT);

    // Bind the socket to the server address by calling the bind() function of the socket
    // bind() returns 0 if the bind was successful
    // bind() returns -1 if the bind was unsuccessful
    if (bind(server_sock, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Failed to bind socket\n";
        return 1;
    }

    //  Listen for connections on the socket by calling the listen() function of the socket
    // listen() returns 0 if the listen was successful
    // listen() returns -1 if the listen was unsuccessful
    if (listen(server_sock, 3) < 0) {
        std::cerr << "Failed to listen for connections\n";
        return 1;
    }

    // Print a message to the console to let the user know that the server is listening for connections
    // The server will continue to listen for connections until the user enters "SHUTDOWN" or "QUIT"
    std::cout << "Server listening on port " << SERVER_PORT << std::endl;

    bool should_shutdown = false;
    while (!should_shutdown) {
        // Accept a connection from a client by calling the accept() function of the socket
        // accept() returns a new socket descriptor if the accept was successful
        // accept() returns -1 if the accept was unsuccessful
        // The new socket descriptor is used to send and receive data to and from the client
        int client_sock = accept(server_sock, NULL, NULL);
        if (client_sock < 0) {
            std::cerr << "Failed to accept connection\n";
            continue;
        }
        // Print a message to the console to let the user know that a new client has connected
        std::cout << "New client connected\n";

        // Create a thread to handle the client
        // We will use a lambda function to create the thread and pass the client socket as a reference to the thread
        // The thread will run in the background and will not block the main thread
        // The thread will run until the user enters "SHUTDOWN" or "QUIT" in the client_handler() function
        // The thread will wait for the user to enter a message as given in the client_handler() function
        std::thread client_thread(client_handler, std::ref(client_sock));

        //  Detach the thread from the main thread
        // This will allow the thread to run in the background and will not block the main thread
        client_thread.detach();
    }

    //  Close the server socket when done with the server
    //  This will allow the server to listen for new connections on the same port
    close(server_sock);
    return 0;
}
