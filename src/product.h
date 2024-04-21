#pragma once

#include "database.h"
#include <functional>
#include <string>
#include <vector>

class Product : public DatabaseObject
{
  public:
    static std::string items;

    Product(
        unsigned int id,
        std::string name,
        double price,
        unsigned int available_count,
        std::string description = ""
    );

    const unsigned int &get_id() const;
    const std::string &get_name() const;
    const double &get_price() const;
    const unsigned int &get_available_count() const;
    const std::string &get_description() const;
    const void set_name(const std::string name);
    const void set_price(const double price);
    const void set_available_count(const unsigned int available_count);
    const void set_description(const std::string description);

    std::function<void(Product)> on_change_callback;

  protected:
    inline void call_callbacks() const;

    unsigned int id;
    std::string name;
    double price;
    unsigned int available_count;
    std::string description;
};

class Products : protected Database
{
  public:
    Products(std::string path);
    void add_product(
        const std::string &name,
        const double &price,
        const unsigned int &available_count,
        const std::string &description
    );
    const Product *get_product(const unsigned int &id) const;
    const std::vector<Product> &get_sold_out_products() const;
    const std::vector<Product> &list_products(const unsigned int &id) const;
    const bool save_database() const;
    const bool save_database(std::string path) const;
    void set_database_path(std::string path);
    static Products *load_from_database(std::string path);
};
