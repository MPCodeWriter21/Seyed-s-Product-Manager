#include "argparse.h"
#include "database.h"
#include <iostream>
#include <string>

int main(int argc, char *argv[])
{
    ArgumentParser parser("Main Parser", "Seyed's Product Manager");
    parser.add_argument(
        new std::string[]{"-t", "--table"}, 2, TYPE::STRING, "table", nullptr,
        "Table Name"
    );
    auto args = parser.parse_args(argc, argv);
    std::string *table_name = (std::string *)args.get("table");
    if (table_name == nullptr)
        parser.parser_error("You must specify the table name. (Use -t)");
    std::cout << "Table name is: " << *table_name << std::endl;
    Database db;
    db.open_db("./db.sqlite3");
    db.create_table(*table_name, "id INTEGER PRIMARY KEY, NAME TEXT");
    std::cout << "Done!\n";
    return 0;
}
