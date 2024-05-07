#pragma once

#include "sqlite/sqlite3.h"
#include <string>
#include <vector>

using Record = std::vector<std::string>;

class Database
{
    friend class Users;

  public:
    Database();
    Database(const char *path);
    int open_db(const char *path);
    void close_db();
    std::vector<Record> execute(const std::string &command);
    std::vector<Record> execute(const std::string &command, const std::string values[]);
    int create_table(
        const std::string &table_name, const std::string &content, bool exists_ok = true
    );
    ~Database();

    char *error_message;

  protected:
    void database_error(const std::string &message) const;

    sqlite3 *db;
    bool is_available;
    int status_code;
};

class DatabaseObject
{
  protected:
    /* Example items:
     * "id INTEGER PRIMARY KEY AUTOINCREMENT, "
     * "first_name TEXT NOT NULL, "
     * "last_name TEXT NOT NULL, "
     * "age INT"
     */
    DatabaseObject(std::string items);
    std::string items_string;
};
