#include "database.hpp"
#include "product.hpp"
#include "sqlite/sqlite3.h"
#include <iostream>
#include <string>
#include <vector>

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
    std::string description,
    std::function<void(Product &)> on_change_callback
)
    : DatabaseObject(items), on_change_callback(on_change_callback), id(id), name(name),
      price(price), available_count(available_count), description(description)
{
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

void Product::set_name(const std::string name)
{
    this->name = name;
    call_callbacks();
}
void Product::set_price(const double price)
{
    this->price = price;
    call_callbacks();
}
void Product::set_available_count(const unsigned int available_count)
{
    this->available_count = available_count;
    call_callbacks();
}
void Product::set_description(const std::string description)
{
    this->description = description;
    call_callbacks();
}

inline void Product::call_callbacks()
{
    if (this->on_change_callback != nullptr)
        on_change_callback(*this);
}

void Product::show_info() const
{
    std::cout << "Product ID               : " << get_id() << std::endl;
    std::cout << "Name                     : " << get_name() << std::endl;
    std::cout << "Price                    : " << get_price() << std::endl;
    std::cout << "No. of Available Products: " << get_available_count() << std::endl;
    std::cout << "Product Descriptions     : " << get_description() << std::endl;
}

// Products

Products::Products(std::string path) : Database(path.c_str())
{
    create_table("products", Product::items, true);
}

Products::Products(Products &products) : Database(products)
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
        database_error("Error adding new product: " + (std::string)error_message);
}

Product *Products::get_product(const unsigned int &id)
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
            std::stoi(product_info[3]), product_info[4],
            [this](Product &p) { this->save_changed_product(p); }
        );
    }
    return nullptr;
}

const std::vector<Product> &Products::list_products()
{
    std::vector<Product> *products = new std::vector<Product>();
    std::vector<Record> records = execute("SELECT * FROM products");
    for (unsigned int i = 0; i < records.size(); i++)
    {
        const Record &product_info = records[i];
        products->push_back(Product(
            std::stoi(product_info[0]), product_info[1], std::stod(product_info[2]),
            std::stoi(product_info[3]), product_info[4],
            [this](Product &p) { this->save_changed_product(p); }
        ));
    }
    return *products;
}

const std::vector<Product> &Products::get_sold_out_products()
{
    std::vector<Product> *products = new std::vector<Product>();
    std::vector<Record> records =
        execute("SELECT * FROM products WHERE available_count=0");
    for (unsigned int i = 0; i < records.size(); i++)
    {
        const Record &product_info = records[i];
        products->push_back(Product(
            std::stoi(product_info[0]), product_info[1], std::stod(product_info[2]),
            std::stoi(product_info[3]), product_info[4],
            [this](Product &p) { this->save_changed_product(p); }
        ));
    }
    return *products;
}

void Products::set_database_path(std::string path)
{
    this->open_db(path.c_str());
}

void Products::save_changed_product(Product &product)
{
    execute(
        "UPDATE products SET name=\"" + product.get_name() +
        "\", price=" + std::to_string(product.get_price()) + ", available_count=" +
        std::to_string(product.get_available_count()) + ", description=\"" +
        product.get_description() + "\" WHERE id=" + std::to_string(product.get_id())
    );
}
