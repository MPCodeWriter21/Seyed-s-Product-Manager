#include "sqlite/sqlite3.h"
#include <string>

class Database
{
  public:
    Database();
    Database(const char *path);
    int open_db(const char *path);
    void close_db();
    int execute(const std::string &command);
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
