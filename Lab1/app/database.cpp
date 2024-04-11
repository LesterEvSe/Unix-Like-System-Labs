#include "database.hpp"

int Database::callback(void *data, int argc, char **argv, char **azColName) {
    std::vector<std::string> *messages = static_cast<std::vector<std::string>*>(data);
    for(int i = 0; i < argc; i++)
        messages->push_back(argv[i] ? argv[i] : "NULL");
    return 0;
}

int Database::callback_last_id(void *data, int argc, char **argv, char **azColName) {
    int *last_id_ptr = static_cast<int*>(data);
    if (argc > 0 && argv[0]) {
        *last_id_ptr = std::stoi(argv[0]);
    }
    return 0;
}


Database::Database(const std::string &dbname) {
    int rc = sqlite3_open(dbname.c_str(), &database);

    if (rc) {
        std::cerr << "Can't open database: " << sqlite3_errmsg(database) << std::endl;
        sqlite3_close(database);
        exit(1);
    }

    std::string sql = "CREATE TABLE IF NOT EXISTS messages (ID INTEGER PRIMARY KEY AUTOINCREMENT, message TEXT);";
    rc = sqlite3_exec(database, sql.c_str(), callback, 0, nullptr);

    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << sqlite3_errmsg(database) << std::endl;
        sqlite3_close(database);
        exit(1);
    }
}

Database::~Database() {
    sqlite3_close(database);
}

void Database::emplace_back(const std::string &message) {
    std::string sql = "INSERT INTO messages (message) VALUES ('" + message + "');";
    int rc = sqlite3_exec(database, sql.c_str(), nullptr, 0, nullptr);

    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << sqlite3_errmsg(database) << std::endl;
        sqlite3_close(database);
        exit(1);
    }
}

std::vector<std::string> Database::last_messages(int number) {
    std::vector<std::string> messages;
    // Вычисляем ID последнего сообщения
    std::string sql_last_id = "SELECT MAX(ID) FROM messages;";
    int last_id = 0;
    int rc = sqlite3_exec(database, sql_last_id.c_str(), callback_last_id, &last_id, nullptr);

    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << sqlite3_errmsg(database) << std::endl;
        sqlite3_close(database);
        exit(1);
    }


    // Выбираем последние number сообщений, начиная с ID равного (last_id - number + 1)
    if (last_id > 0) {
        int start_id = std::max(1, last_id - number + 1);
        std::string sql = "SELECT message FROM messages WHERE ID >= " + std::to_string(start_id) + ";";
        rc = sqlite3_exec(database, sql.c_str(), callback, &messages, nullptr);

        if (rc != SQLITE_OK) {
            std::cerr << "SQL error: " << sqlite3_errmsg(database) << std::endl;
            sqlite3_close(database);
            exit(1);
        }
    }

    return messages;
}
