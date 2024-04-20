#include "sqlite/sqlite3.h"
#include <string>

class Database
{
  public:
    Database();
    Database(const char *path);
    int open_db(const char *path);
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
