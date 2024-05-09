#include "database.hpp"
#include "sqlite/sqlite3.h"
#include "utils/exceptions.hpp"
#include "utils/utils.hpp"
#include "utils/warnings.hpp"
#include <exception>
#include <iostream>
#include <string>
#include <vector>

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
    if (!is_available)
    {
        database_error("Failed to open database at `" + (std::string)path + "`!");
    }
    return status_code;
}

void Database::close_db()
{
    if (db != nullptr)
        sqlite3_close(db);
}

DISABLE_WARNING_PUSH
DISABLE_WARNING_UNREFERENCED_FORMAL_PARAMETER
int _select_callback(
    void *data, int num_fields, char **fields, char **col_names
) DISABLE_WARNING_POP
{
    std::vector<Record> *records = static_cast<std::vector<Record> *>(data);
    try
    {
        records->emplace_back(fields, fields + num_fields);
    }
    catch (...)
    {
        // abort select on failure, don't let exception propogate thru sqlite3
        // call-stack
        return 1;
    }
    return 0;
}

/* Runs the command and replaces un-escaped question marks(?) in the command with the
 * given `values`.
 * + Values will be escaped automatically.
 * + Number of `values` should equal the number of question marks in the command
 */
std::vector<Record> Database::execute(
    const std::string &command, const std::initializer_list<std::string> values
)
{
    size_t index_for_values = 0, i = 0;
    std::string processed_command = "";
    if (values.size() == 0)
    {
        processed_command = command;
        i = command.length();
    }
    for (; i < command.length(); i++)
    {
        if (command[i] == '?')
        {
            if (i == index_for_values)
                database_error("More question marks than `values`!");
            if (i == 0)
                // This wouldn't make sense to have and will create a sql error but
                // I just wanted to make it a thing LOL
                processed_command += '?';
            else if (command[i - 1] == '\\')
            {
                processed_command =
                    processed_command.substr(0, processed_command.length() - 1) + '?';
            }
            else
            {
                processed_command +=
                    '"' + escape_string(values.begin()[index_for_values]) + '"';
                index_for_values++;
            }
        }
        else
            processed_command += command[i];
    }
    std::vector<Record> records;
    status_code = sqlite3_exec(
        db, processed_command.c_str(), _select_callback, &records, &error_message
    );
    for (size_t i = 0; i < records.size(); i++)
    {
        for (size_t j = 0; j < records[i].size(); j++)
        {
            records[i][j] = unescape_string(records[i][j]);
        }
    }
    return records;
}

std::vector<Record> Database::execute(
    const std::string &command, const std::vector<std::string> &values
)
{
    size_t index_for_values = 0, i = 0;
    std::string processed_command = "";
    if (values.size() == 0)
    {
        processed_command = command;
        i = command.length();
    }
    for (; i < command.length(); i++)
    {
        if (command[i] == '?')
        {
            if (i == index_for_values)
                error("More question marks than `values`!");
            if (i == 0)
                // This wouldn't make sense to have and will create a sql error but
                // I just wanted to make it a thing LOL
                processed_command += '?';
            else if (command[i - 1] == '\\')
            {
                processed_command =
                    processed_command.substr(0, processed_command.length() - 1) + '?';
            }
            else
            {
                processed_command +=
                    '"' + escape_string(values.begin()[index_for_values]) + '"';
                index_for_values++;
            }
        }
        else
            processed_command += command[i];
    }
    std::cout << processed_command << std::endl;
    std::vector<Record> records;
    status_code = sqlite3_exec(
        db, processed_command.c_str(), _select_callback, &records, &error_message
    );
    for (size_t i = 0; i < records.size(); i++)
    {
        for (size_t j = 0; j < records[i].size(); j++)
        {
            records[i][j] = unescape_string(records[i][j]);
        }
    }
    return records;
}

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
int Database::create_table(
    const std::string &table_name, const std::string &content, bool exists_ok
)
{
    std::string command = "CREATE TABLE ";
    if (exists_ok)
        command += "IF NOT EXISTS ";
    command += table_name;
    command += "(";
    command += content;
    command += ")";
    status_code = sqlite3_exec(db, command.c_str(), NULL, 0, &error_message);
    if (status_code != SQLITE_OK)
    {
        database_error("Error creating table `" + table_name + "`: " + error_message);
        sqlite3_free(error_message);
    }
    return status_code;
}

void Database::database_error(const std::string &message) const
{
    error("Database Error: " + message);
}

Database::~Database()
{
    this->close_db();
}

// DatabaseObject

DatabaseObject::DatabaseObject(std::string items)
{
    this->items_string = items;
}
