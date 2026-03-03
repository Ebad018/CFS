#pragma once
#include <string>
#include <vector>
#include <memory>
// Forward declaration to avoid including sqlite3.h in header if possible, 
// but for standard usage usually we include it or use void*.
struct sqlite3;
struct sqlite3_stmt;

class Database {
public:
    Database();
    ~Database();

    bool Connect(const std::string& dbPath);
    void Disconnect();

    // Basic query execution (no return data)
    bool Execute(const std::string& sql);

    // For initialization
    void InitializeTables();

private:
    sqlite3* db = nullptr;
};
