#include "database.h"
#include "sqlite/sqlite3.h"

Database::Database()
{
    status_code = 0;
    is_available = 0;
    db = nullptr;
}

Database::Database(const char *path) : Database()
{
    open_db(path);
}

int Database::open_db(const char *path)
{
    status_code = sqlite3_open(path, &db);
    is_available = status_code == SQLITE_OK;
    return status_code;
}

int Database::execute(const std::string &command)
{
    status_code = sqlite3_exec(db, command.c_str(), NULL, 0, &error_message);
    return status_code;
}

int Database::create_table(
    const std::string &table_name, const std::string &content, bool exists_ok
)
{
    /* Usage example:
     *
     * db.create_table(
     * "students",
     * "id INTEGER PRIMARY KEY AUTOINCREMENT, "
     * "first_name TEXT NOT NULL, "
     * "last_name TEXT NOT NULL, "
     * "age INT",
     * true
     * )
     */
    std::string command = "CREATE TABLE ";
    if (exists_ok)
        command += "IF NOT EXISTS ";
    command += table_name;
    command += "(";
    command += content;
    command += ")";
    status_code = sqlite3_exec(db, command.c_str(), NULL, 0, &error_message);
    return status_code;
}

Database::~Database()
{
    if (db != nullptr)
        sqlite3_close(db);
}
