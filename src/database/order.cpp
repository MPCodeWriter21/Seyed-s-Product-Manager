#include "database.hpp"
#include "database/product.hpp"
#include "order.hpp"
#include "sqlite/sqlite3.h"
#include "utils/exceptions.hpp"
#include <iomanip>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

// ProductOrder

ProductOrder::ProductOrder(const Product &product, const unsigned int &count)
    : product(product), count(count)
{
}

std::string ProductOrder::to_string() const
{
    return std::to_string(product.get_id()) + "," + std::to_string(count);
}

ProductOrder ProductOrder::from_string(const std::string &data, Products &products)
{
    unsigned long long index;
    unsigned int product_id, count;
    if ((index = data.find(",")) == std::string::npos)
        error("Invalid content for ProductOrder: `" + data + "`");
    product_id = std::stoi(data.substr(0, index));
    count = std::stoi(data.substr(index + 1, data.size()));
    return ProductOrder(*products.get_product(product_id), count);
}

// Order

std::string Order::items = "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                           "product_orders TEXT NOT NULL, "
                           "is_paid INTEGER DEFAULT 0, "
                           "discount INTEGER DEFAULT 0";

Order::Order(
    unsigned int id,
    const std::vector<ProductOrder> &product_orders,
    const bool &is_paid,
    const int &discount,
    const std::function<void(Order &)> &on_change_callback
)
    : DatabaseObject(items), on_change_callback(on_change_callback), id(id),
      product_orders(product_orders), _is_paid(is_paid)
{
    if (discount < 0)
        error("We cannot have negative discounts.");
    if (discount > 100)
        error("We cannot have a discount greater than 100%");
    this->discount = discount;
}

const unsigned int &Order::get_id() const
{
    return id;
}

double Order::get_total() const
{
    double total = 0;
    for (unsigned int i = 0; i < product_orders.size(); i++)
        total += product_orders[i].product.get_price() * product_orders[i].count;
    return total;
}

double Order::get_total_after_discount() const
{
    return get_total() * (1 - discount / 100.0);
}

const std::vector<ProductOrder> &Order::get_product_orders() const
{
    return product_orders;
}

unsigned int Order::get_count() const
{
    return product_orders.size();
}

void Order::add_product_order(const ProductOrder &product_order)
{
    product_orders.push_back(product_order);
    call_callbacks();
}

void Order::remove_product_order(int index)
{
    product_orders.erase(std::next(product_orders.begin(), index));
    call_callbacks();
}

void Order::set_product_order(int index, const ProductOrder &product_order)
{
    product_orders[index] = product_order;
    call_callbacks();
}

const int &Order::get_discount() const
{
    return discount;
}
void Order::set_discount(const int &discount)
{
    this->discount = discount;
}

const bool &Order::is_paid() const
{
    return _is_paid;
}

bool Order::pay()
{
    if (_is_paid)
        return false;
    for (size_t i = 0; i < product_orders.size(); i++)
    {
        if (product_orders[i].count > product_orders[i].product.get_available_count())
            return false;
    }
    for (size_t i = 0; i < product_orders.size(); i++)
    {
        product_orders[i].product.set_available_count(
            product_orders[i].product.get_available_count() - product_orders[i].count
        );
    }
    _is_paid = true;
    call_callbacks();
    return true;
}

inline void Order::call_callbacks()
{
    if (this->on_change_callback != nullptr)
        on_change_callback(*this);
}

void Order::show_info() const
{
    std::cout << "Order ID               : " << get_id() << std::endl;
    std::cout << "Total price            : " << std::setprecision(2) << std::fixed
              << get_total() << std::endl;
    if (get_total() != get_total_after_discount())
        std::cout << "Total with " << std::setw(2) << discount
                  << "% discount: " << get_total_after_discount() << std::endl;
    std::cout << "Is Paid                : " << (_is_paid ? "Yes" : "No") << std::endl;
    std::cout << "No. of Orders          : " << get_count() << std::endl;
}

std::string Order::to_string(std::vector<ProductOrder> product_orders)
{
    bool escaped = false;
    std::string data = "", order_string, escaped_order_string;
    for (unsigned int i = 0; i < product_orders.size(); i++)
    {
        if (data != "")
            data += ',';
        order_string = product_orders[i].to_string();
        escaped_order_string = "";
        for (char c : order_string)
        {
            if (escaped)
            {
                escaped = false;
                escaped_order_string += c;
                continue;
            }
            if (c == ',')
            {
                escaped_order_string += "\\,";
                continue;
            }
            else if (c == '\\')
                escaped = true;
            escaped_order_string += c;
        }
        data += escaped_order_string;
    }
    return data;
}

std::vector<ProductOrder> Order::from_string(
    const std::string &data, Products &products
)
{
    bool escaped = false;
    std::vector<ProductOrder> product_orders;
    std::string part = "";
    for (char c : data)
    {
        if (escaped)
        {
            part += c;
            escaped = false;
            continue;
        }
        if (c == '\\')
        {
            escaped = true;
            continue;
        }
        if (c == ',')
        {
            product_orders.push_back(ProductOrder::from_string(part, products));
            part = "";
            continue;
        }
        part += c;
    }
    product_orders.push_back(ProductOrder::from_string(part, products));
    return product_orders;
}

std::string Order::to_string() const
{
    return Order::to_string(product_orders);
}

Order Order::from_string(
    const unsigned int id,
    const std::string &data,
    const bool &is_paid,
    Products &products
)
{
    return Order(id, Order::from_string(data, products), is_paid);
}

// Orders

Orders::Orders(std::string path) : Database(path.c_str())
{
    create_table("orders", Order::items, true);
}

Orders::Orders(Orders &orders) : Database(orders)
{
    create_table("orders", Order::items, true);
}

void Orders::add_order(
    const std::vector<ProductOrder> &product_orders, const int &discount
)
{
    if (discount < 0)
        error("We cannot have negative discounts.");
    if (discount > 100)
        error("We cannot have a discount greater than 100%");

    execute(
        "INSERT INTO orders(product_orders, discount) VALUES(?, ?)",
        {Order::to_string(product_orders), std::to_string(discount)}
    );

    if (status_code != SQLITE_OK)
        database_error("Error adding new order: " + (std::string)error_message);
}

Order *Orders::get_order(const unsigned int &id, Products &products)
{
    std::vector<Record> records =
        execute("SELECT * FROM orders WHERE id=?", {std::to_string(id)});

    // Found no order with the id
    if (records.size() < 1)
        return nullptr;
    // Found more than 1 order with the same id. (Should be impossible)
    if (records.size() > 1)
        database_error("Found more than one order with id=" + std::to_string(id));
    else
    {
        Record &order_info = records[0];
        return new Order(
            std::stoi(order_info[0]), Order::from_string(order_info[1], products),
            std::stoi(order_info[2]), std::stoi(order_info[3]),
            [this](Order &p) { this->save_changed_order(p); }
        );
    }
    return nullptr;
}

const std::vector<Order> &Orders::list_orders(Products &products)
{
    std::vector<Order> *orders = new std::vector<Order>();
    std::vector<Record> records = execute("SELECT * FROM orders");
    for (unsigned int i = 0; i < records.size(); i++)
    {
        const Record &order_info = records[i];
        orders->push_back(Order(
            std::stoi(order_info[0]), Order::from_string(order_info[1], products),
            std::stoi(order_info[2]), std::stoi(order_info[3]),
            [this](Order &p) { this->save_changed_order(p); }
        ));
    }
    return *orders;
}

void Orders::set_database_path(std::string path)
{
    this->open_db(path.c_str());
}

void Orders::save_changed_order(Order &order)
{
    execute(
        "UPDATE orders SET product_orders=?, is_paid=?, discount=? WHERE id=?",
        {order.to_string(), std::to_string((int)order.is_paid()),
         std::to_string(order.get_discount()), std::to_string(order.get_id())}
    );
}
