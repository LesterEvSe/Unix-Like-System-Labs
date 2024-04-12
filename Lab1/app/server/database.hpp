#ifndef SERVER_DATABASE_HPP
#define SERVER_DATABASE_HPP

#include <iostream>
#include <vector>
#include <string>
#include <sqlite3.h>

class Database {
private:
    sqlite3 *database;

    static int callback(void *data, int argc, char **argv, char **azColName);
    static int callback_last_id(void *data, int argc, char **argv, char **azColName);

public:
    Database(const std::string& dbname);
    ~Database();

    void emplace_back(const std::string &message);
    std::vector<std::string> last_messages(int number=10);
};

#endif //SERVER_DATABASE_HPP
