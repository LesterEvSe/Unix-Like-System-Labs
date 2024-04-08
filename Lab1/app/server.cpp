#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <vector>
#include <thread>
#include <mutex>
#include <sqlite3.h>

class DB
{
private:
    sqlite3 *db;

    static int callback(void *data, int argc, char **argv, char **azColName) {
        for(int i = 0; i < argc; i++)
            std::cout << azColName[i] << ": " << (argv[i] ? argv[i] : "NULL") << '\n';
        return 0;
    }

public:
    DB(const std::string& dbname) {
        int rc = sqlite3_open(dbname.c_str(), &db);

        if (rc) {
            std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
            sqlite3_close(db);
            exit(1);
        }

        std::string sql = "CREATE TABLE IF NOT EXISTS messages (ID INTEGER PRIMARY KEY AUTOINCREMENT, message TEXT);";
        rc = sqlite3_exec(db, sql.c_str(), callback, 0, nullptr);

        if (rc != SQLITE_OK) {
            std::cerr << "SQL error: " << sqlite3_errmsg(db) << std::endl;
            sqlite3_close(db);
            exit(1);
        }
    }

    ~DB() {
        sqlite3_close(db);
    }

    void emplace_back(const std::string& message) {
        std::string sql = "INSERT INTO messages (message) VALUES ('" + message + "');";
        int rc = sqlite3_exec(db, sql.c_str(), nullptr, 0, nullptr);

        if (rc != SQLITE_OK) {
            std::cerr << "SQL error: " << sqlite3_errmsg(db) << std::endl;
            sqlite3_close(db);
            exit(1);
        }
    }

    std::vector<std::string> messages(int start_id=0) {
        std::vector<std::string> messages;
        std::string sql = "SELECT message FROM messages WHERE ID >= " + std::to_string(start_id) + ";";
        int rc = sqlite3_exec(db, sql.c_str(), callback, &messages, nullptr);

        if (rc != SQLITE_OK) {
            std::cerr << "SQL error: " << sqlite3_errmsg(db) << std::endl;
            sqlite3_close(db);
            exit(1);
        }

        return messages;
    }
};


void handleClient(int clientSocket) {
    char buffer[1024];
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0) {
            std::cerr << "Connection closed\n";
            break;
        }

        std::string message = buffer;
        if (message.find("GET /") != std::string::npos) {
            // Отправить клиенту HTTP-ответ с кодом 200 для подтверждения успешного соединения
            const char* response = "HTTP/1.1 200 OK\r\nContent-Length: 0\r\n\r\n";
            send(clientSocket, response, strlen(response), 0);
            continue; // Продолжаем прослушивать соединение после отправки ответа на GET-запрос
        }

        // Вывод принятого сообщения в консоль
        std::cout << "Received message from client: " << message << std::endl;

        // Отправка обратно клиенту принятого сообщения
        send(clientSocket, message.c_str(), message.size(), 0);
    }

    // Закрытие сокета клиента
    close(clientSocket);
}

int main() {
    // Создание серверного сокета
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Error creating server socket\n";
        return 1;
    }

    const int port = 8080;

    // Настройка адреса и порта сервера
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(port);

    // Привязка сокета к адресу и порту
    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1) {
        std::cerr << "Error binding server socket\n";
        return 1;
    }

    // Начало прослушивания входящих соединений
    if (listen(serverSocket, SOMAXCONN) == -1) {
        std::cerr << "Error listening on server socket\n";
        return 1;
    }

    std::cout << "Server is running on port " << port << "\n";

    DB db("messages.sqlite");


    for (std::string str : db.messages())
        std::cout << str << std::endl;

    // Принятие входящих соединений и обработка каждого клиента в отдельном потоке
    while (true) {
        sockaddr_in clientAddress;
        socklen_t clientAddressSize = sizeof(clientAddress);
        int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientAddressSize);
        if (clientSocket == -1) {
            std::cerr << "Error accepting connection\n";
            continue;
        }

        std::cout << "New connection established\n";

        // Запуск потока для обработки клиента
        std::thread clientThread(handleClient, clientSocket);
        clientThread.detach();
    }

    // Закрытие серверного сокета
    close(serverSocket);

    return 0;
}
