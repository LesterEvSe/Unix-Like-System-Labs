#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <thread>
#include <chrono>
#include <sstream> // for output


void send_message(const std::string &username, int client_socket) {
    static const std::string REQUEST = "GET_MESSAGES";
    char buffer[1024];

    while (true)
    {

        std::string message;
        std::cout << username << ": ";

        std::getline(std::cin, message);
        if (message.empty()) continue; // Skip empty messages

        if (message == "EXIT") break;
        if (message != "GET") {
            message = username + ": " + message;
            send(client_socket, message.c_str(), message.size(), 0);
            continue;
        }

        std::cout << std::endl << "----- HISTORY -----\n";
        send(client_socket, REQUEST.c_str(), REQUEST.size(), 0);
        memset(buffer, 0, sizeof(buffer));
        int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);

        if (bytes_received <= 0) {
            std::cerr << "Connection closed\n";
            break;
        }

        std::cout << buffer;
        std::cout << "----- END -----\n" << std::endl;
    }
}

int main() {
    // Создание клиентского сокета
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        std::cerr << "Error creating client socket\n";
        return 1;
    }

    // Server set up
    const char *serverIP = "server"; //"127.0.0.1";
    const int serverPort = 2222;

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(serverPort);
    inet_pton(AF_INET, serverIP, &serverAddress.sin_addr);

    // Подключение к серверу
    if (connect(clientSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1) {
        std::cerr << "Error connecting to server\n'Enter' for exit\n";
        std::string trash;
        std::getline(std::cin, trash);
        return 1;
    }

    system("clear");
    std::cout << "Enter username: ";
    std::string username;
    std::getline(std::cin, username);
    send_message(username, clientSocket);

    close(clientSocket);
    return 0;
}
