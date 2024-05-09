#pragma once

#include "database.hpp"
#include "product.hpp"
#include <functional>
#include <string>
#include <vector>

class ProductOrder
{
  public:
    ProductOrder(const Product &, const unsigned int &);
    std::string to_string() const;
    static ProductOrder from_string(const std::string &data, Products &products);

    Product product;
    unsigned int count;
};

class Orders;

class Order : public DatabaseObject
{
  public:
    static std::string items;

    Order(
        unsigned int id,
        const std::vector<ProductOrder> &product_orders,
        Orders &parent,
        const double &pay_date = 0,
        const int &discount = 0,
        const std::function<void(Order &)> &on_change_callback = nullptr
    );

    const unsigned int &get_id() const;
    double get_total() const;
    double get_total_after_discount() const;
    const std::vector<ProductOrder> &get_product_orders() const;
    unsigned int get_count() const;
    void add_product_order(const ProductOrder &product_order);
    void remove_product_order(int index);
    void set_product_order(int index, const ProductOrder &product_order);
    const int &get_discount() const;
    void set_discount(const int &discount);
    const double &get_pay_julian_day() const;
    std::string get_pay_date() const;
    std::string get_pay_time() const;
    bool is_paid() const;
    bool pay();
    void show_info() const;
    static std::string to_string(std::vector<ProductOrder> product_orders);
    static std::vector<ProductOrder> from_string(
        const std::string &data, Products &products
    );
    std::string to_string() const;
    static Order from_string(
        const unsigned int id,
        const std::string &data,
        Orders &parent,
        const double &pay_date,
        Products &products,
        const int &discount
    );

    std::function<void(Order &)> on_change_callback;

  protected:
    inline void call_callbacks();

    unsigned int id;
    std::vector<ProductOrder> product_orders;
    double pay_date;
    int discount;
    Orders &parent;
};

class Orders : protected Database
{
    friend class Order;

  public:
    Orders(std::string path);
    Orders(Orders &products);
    void add_order(
        const std::vector<ProductOrder> &product_orders, const int &discount
    );
    Order *get_order(const unsigned int &id, Products &products);
    std::vector<Order> get_orders(
        const std::initializer_list<std::string> &from_date,
        const std::initializer_list<std::string> &to_date,
        Products &products
    );
    std::vector<Order> list_orders(Products &products);
    void set_database_path(std::string path);

  protected:
    void save_changed_order(Order &);
};
