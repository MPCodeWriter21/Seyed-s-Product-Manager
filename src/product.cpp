#include "database.h"
#include "product.h"
#include "sqlite/sqlite3.h"
#include <iostream>
#include <string>

// Product

std::string Product::items = "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                             "name TEXT NOT NULL, "
                             "price REAL, "
                             "available_count INTEGER NOT NULL, "
                             "description TEXT";

Product::Product(
    unsigned int id,
    std::string name,
    double price,
    unsigned int available_count,
    std::string description
)
    : DatabaseObject(items)
{
    this->id = id;
    this->name = name;
    this->price = price;
    this->available_count = available_count;
    this->description = description;
}

const unsigned int &Product::get_id() const
{
    return id;
}

const std::string &Product::get_name() const
{
    return name;
}

const double &Product::get_price() const
{
    return price;
}

const unsigned int &Product::get_available_count() const
{
    return available_count;
}

const std::string &Product::get_description() const
{
    return description;
}

const void Product::set_name(const std::string name)
{
    this->name = name;
    call_callbacks();
}
const void Product::set_price(const double price)
{
    this->price = price;
    call_callbacks();
}
const void Product::set_available_count(const unsigned int available_count)
{
    this->available_count = available_count;
    call_callbacks();
}
const void Product::set_description(const std::string description)
{
    this->description = description;
    call_callbacks();
}

inline void Product::call_callbacks() const
{
    if (this->on_change_callback != nullptr)
        on_change_callback(*this);
}

// Products

Products::Products(std::string path) : Database(path.c_str())
{
    create_table("products", Product::items, true);
}

void Products::add_product(
    const std::string &name,
    const double &price,
    const unsigned int &available_count,
    const std::string &description
)
{
    execute(
        "INSERT INTO products(name, price, available_count, description) "
        "VALUES(\"" +
        name + "\", " + std::to_string(price) + ", " + std::to_string(available_count) +
        ", \"" + description + "\")"
    );
    if (status_code != SQLITE_OK)
        std::cerr << error_message << std::endl;
}

const Product *Products::get_product(const unsigned int &id)
{
    std::vector<Record> records =
        execute("SELECT * FROM products WHERE id=" + std::to_string(id));

    // Found no product with the id
    if (records.size() < 1)
        return nullptr;
    // Found more than 1 product with the same id. (Should be impossible)
    if (records.size() > 1)
        database_error("Found more than one product with id=" + std::to_string(id));
    else
    {
        Record &product_info = records[0];
        return new Product(
            std::stoi(product_info[0]), product_info[1], std::stod(product_info[2]),
            std::stoi(product_info[3]), product_info[4]
        );
    }
    return nullptr;
}

const std::vector<Product> &Products::list_products(const unsigned int &id) const
{
    // return products;
}

// Returns true on success
const bool Products::save_database() const
{
    // if (!path.empty())
    //     return save_database(path);
    return false;
}

const bool Products::save_database(std::string path) const
{
    // TODO: Implement the database logic
    return false;
}

void Products::set_database_path(std::string path)
{
    // this->path = path;
}

Products *Products::load_from_database(std::string path)
{
    // TODO: Implement the database logic
    return nullptr;
}
