#include "product.h"

// Product

Product::Product(
    unsigned int id, std::string name, double price, std::string description
)
{
    this->id = id;
    this->name = name;
    this->price = price;
    this->description = description;
}

const unsigned int &Product::get_id() const
{
    return id;
}

// Products

void Products::add_product(const Product product)
{
    products.push_back(product);
}

const Product *Products::get_product(const unsigned int &id) const
{
    for (unsigned int i = 0; i < products.size(); i++)
    {
        if (products[i].get_id() == id)
            return &products[i];
    }
    return nullptr;
}

const std::vector<Product> &Products::list_products(const unsigned int &id) const
{
    return products;
}

// Returns true on success
const bool Products::save_database() const
{
    if (!path.empty())
        return save_database(path);
    return false;
}

const bool Products::save_database(std::string path) const
{
    // TODO: Implement the database logic
    return false;
}

void Products::set_database_path(std::string path)
{
    this->path = path;
}

Products *Products::load_from_database(std::string path)
{
    // TODO: Implement the database logic
    return nullptr;
}
