#include "database.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <thread>
#include <mutex>

const std::string GET = "GET_MESSAGES";
Database db("messages.sqlite");

void handleClient(int client_socket) {
    char buffer[1024];
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytesReceived = recv(client_socket, buffer, sizeof(buffer), 0);

        if (bytesReceived <= 0) {
            std::cerr << "Connection closed\n";
            break;
        }

        std::string message = buffer;
        if (message != GET) {
            db.emplace_back(message);
            continue;
        }

        std::string message_to_send;
        for (const std::string &msg : db.last_messages())
            message_to_send += msg + "\n";
        send(client_socket, message_to_send.c_str(), message_to_send.size(), 0);
    }
    close(client_socket);
}


int main() {
    // Create server socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Error creating server socket\n";
        return 1;
    }

    const int port = 8080;

    // Server set up
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(port);

    // Binding a socket to an address and port
    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1) {
        std::cerr << "Error binding server socket\n";
        return 1;
    }

    // Start listening messages
    if (listen(serverSocket, SOMAXCONN) == -1) {
        std::cerr << "Error listening on server socket\n";
        return 1;
    }
    std::cout << "Server is running on port " << port << "\n";

    for (std::string str : db.last_messages())
        std::cout << str << std::endl;

    // Get connections and processing each client in a separate thread
    while (true) {
        sockaddr_in clientAddress;
        socklen_t clientAddressSize = sizeof(clientAddress);
        int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientAddressSize);
        if (clientSocket == -1) {
            std::cerr << "Error accepting connection\n";
            continue;
        }
        std::cout << "New connection established\n";

        // Start processing the client
        std::thread clientThread(handleClient, clientSocket);
        clientThread.detach();
    }

    close(serverSocket);
    return 0;
}
