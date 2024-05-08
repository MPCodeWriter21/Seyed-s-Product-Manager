#pragma once

#include "database.hpp"
#include <functional>
#include <string>
#include <vector>

enum Permissions
{
    // Change Permissions, Manage Users, Add Product, Update Product, Remove Product,
    // Buy Product
    ROOT = 0b111111,
    CHANGE_PERMISSIONS = 0b100000,
    MANAGE_USERS = 0b010000,
    ADD_PRODUCT = 0b001000,
    UPDATE_PRODUCT = 0b000100,
    REMOVE_PRODUCT = 0b000010,
    BUY_PRODUCT = 0b000001
};

class Password
{
    Password(std::string password);

  public:
    static Password from_string(const std::string &password);
    static Password from_hash(const std::string &password);

    std::string to_string() const;
    bool operator==(const Password &other) const;

  private:
    std::string hashed_password;
};

class User : public DatabaseObject
{
    friend class Users;

  public:
    static std::string items;

    User(
        unsigned int id,
        std::string username,
        Password password,
        std::string name,
        double balance,
        Permissions permissions,
        std::function<void(User &)> on_change_callback = nullptr
    );

    const unsigned int &get_id() const;
    const std::string &get_username() const;
    const std::string &get_name() const;
    const double &get_balance() const;
    const Permissions &get_permissions() const;
    void set_username(const std::string username);
    bool set_password(const std::string username, const Password &old_password);
    bool check_password(const Password &password) const;
    void set_name(const std::string name);
    void set_balance(const double price);
    void add_balance(const double price);
    void sub_balance(const double price);
    void set_permissions(const Permissions permissions);
    void show_info() const;

    std::function<void(User &)> on_change_callback;

  protected:
    inline void call_callbacks();

    unsigned int id;
    std::string username;
    Password password;
    std::string name;
    double balance;
    Permissions permissions;
};

class Users : protected Database
{
  public:
    Users(
        std::string path,
        const std::string &username = "Seyed",
        std::string *password = nullptr
    );
    Users(
        Database &db,
        const std::string &username = "Seyed",
        std::string *password = nullptr
    );
    void add_user(
        const std::string &username,
        const Password &password,
        const std::string &name,
        const double &balance,
        const Permissions &permissions
    );
    User *get_user(const unsigned int &id);
    User *get_user(const std::string &username);
    const std::vector<User> &list_users();
    const std::vector<User> &get_users_by_permissions(const Permissions &permissions);
    void set_database_path(std::string path);

  protected:
    void save_changed_user(User &);
};
