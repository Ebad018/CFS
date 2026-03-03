#include "Database.h"
#include <sqlite3.h>
#include <iostream>

Database::Database() {
}

Database::~Database() {
    Disconnect();
}

bool Database::Connect(const std::string& dbPath) {
    int exit = sqlite3_open(dbPath.c_str(), &db);
    if (exit != SQLITE_OK) {
        std::cerr << "[Database] Error opening DB: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }
    std::cout << "[Database] Connected to " << dbPath << std::endl;
    return true;
}

void Database::Disconnect() {
    if (db) {
        sqlite3_close(db);
        db = nullptr;
        std::cout << "[Database] Disconnected." << std::endl;
    }
}

bool Database::Execute(const std::string& sql) {
    if (!db) return false;
    char* messaggeError;
    int exit = sqlite3_exec(db, sql.c_str(), NULL, 0, &messaggeError);

    if (exit != SQLITE_OK) {
        std::cerr << "[Database] Error Execute: " << messaggeError << std::endl;
        sqlite3_free(messaggeError);
        return false;
    }
    return true;
}

void Database::InitializeTables() {
    std::string sql = "CREATE TABLE IF NOT EXISTS SECTS("
                      "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
                      "NAME           TEXT    NOT NULL, "
                      "GRADE          INT     NOT NULL, "
                      "X              INT     NOT NULL, "
                      "Y              INT     NOT NULL, "
                      "Z              INT     NOT NULL);";
    Execute(sql);
    std::cout << "[Database] Tables initialized." << std::endl;
}
