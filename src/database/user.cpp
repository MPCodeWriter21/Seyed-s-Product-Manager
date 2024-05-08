#include "database.hpp"
#include "sqlite/sqlite3.h"
#include "user.hpp"
#include <cstdlib>
#include <iostream>
#include <ostream>
#include <string>
#include <vector>

// Password

Password::Password(std::string password)
{
    this->hashed_password = password;
}

Password Password::from_string(const std::string &password)
{
    // TODO: Return SHA256 Hash instead of a rot13
    std::string rot13 = "";
    for (char c : password)
    {
        if (c >= 'a' && c <= 'z')
            rot13 += ((c - 'a') + 13) % 26 + 'a';
        else if (c >= 'A' && c <= 'Z')
            rot13 += ((c - 'A') + 13) % 26 + 'A';
        else
            rot13 += c;
    }
    return Password(rot13);
}
Password Password::from_hash(const std::string &password)
{
    return Password(password);
}

std::string Password::to_string() const
{
    // TODO: Make this return SHA256 hash
    return hashed_password;
}

bool Password::operator==(const Password &other) const
{
    return hashed_password == other.hashed_password;
}

// User

std::string User::items = "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                          "username TEXT NOT NULL UNIQUE, "
                          "password TEXT NOT NULL, "
                          "name TEXT NOT NULL, "
                          "balance REAL, "
                          "permissions INTEGER NOT NULL";

User::User(
    unsigned int id,
    std::string username,
    Password password,
    std::string name,
    double balance,
    Permissions permissions,
    std::function<void(User &)> on_change_callback
)
    : DatabaseObject(items), on_change_callback(on_change_callback), id(id),
      username(username), password(password), name(name), balance(balance),
      permissions(permissions)
{
}

const unsigned int &User::get_id() const
{
    return id;
}

const std::string &User::get_username() const
{
    return username;
}

const std::string &User::get_name() const
{
    return name;
}

const double &User::get_balance() const
{
    return balance;
}

const Permissions &User::get_permissions() const
{
    return permissions;
}

void User::set_username(const std::string username)
{
    this->username = username;
    call_callbacks();
}

bool User::set_password(const std::string password, const Password &old_password)
{
    if (!check_password(old_password))
        return false;
    this->password = Password::from_string(password);
    call_callbacks();
    return true;
}

bool User::check_password(const Password &password) const
{
    return password == this->password;
}

void User::set_name(const std::string name)
{
    this->name = name;
    call_callbacks();
}

void User::set_balance(const double balance)
{
    this->balance = balance;
    call_callbacks();
}

void User::add_balance(const double value)
{
    this->balance += value;
    call_callbacks();
}

void User::sub_balance(const double value)
{
    this->balance -= value;
    call_callbacks();
}

void User::set_permissions(const Permissions permissions)
{
    this->permissions = permissions;
    call_callbacks();
}

inline void User::call_callbacks()
{
    if (this->on_change_callback != nullptr)
        on_change_callback(*this);
}

void User::show_info() const
{
    std::cout << "User ID     : " << get_id() << std::endl;
    std::cout << "Username    : " << get_username() << std::endl;
    std::cout << "Name        : " << get_name() << std::endl;
    std::cout << "Balance     : " << get_balance() << std::endl;
    std::cout << "Permissions : " << get_permissions() << std::endl;
}

// Users

Users::Users(std::string path, const std::string &username, std::string *password)
    : Database(path.c_str())
{
    if (password != nullptr)
    {
        srand(time(0));
        std::vector<Record> records =
            execute("SELECT name FROM sqlite_master WHERE type='table' AND name='users'"
            );
        if (records.size() < 1)
        {
            create_table("users", User::items, true);
            // Create the Root user with the submitted username and a random password
            char length = (rand() % 10) + 10;
            for (char i = 0; i < length; i++)
            {
                *password += (rand() % 94) + 33;
            }
            add_user(
                username, Password::from_string(*password), username, 0,
                Permissions::ROOT
            );
        }
    }
    create_table("users", User::items, true);
}

Users::Users(Database &db, const std::string &username, std::string *password)
{
    this->db = db.db;

    if (password != nullptr)
    {
        srand(time(0));
        std::vector<Record> records =
            execute("SELECT name FROM sqlite_master WHERE type='table' AND name='users'"
            );
        if (records.size() < 1)
        {
            create_table("users", User::items, true);
            // Create the Root user with the submitted username and a random password
            char length = (rand() % 10) + 10;
            for (char i = 0; i < length; i++)
            {
                *password += (rand() % 94) + 33;
            }
            add_user(
                username, Password::from_string(*password), username, 0,
                Permissions::ROOT
            );
        }
    }
    create_table("users", User::items, true);
}

void Users::add_user(
    const std::string &username,
    const Password &password,
    const std::string &name,
    const double &balance,
    const Permissions &permissions
)
{
    execute(
        "INSERT INTO users(username, password, name, balance, permissions) VALUES(?, "
        "?, ?, ?, ?)",
        {username, password.to_string(), name, std::to_string(balance),
         std::to_string(permissions)}
    );
    if (status_code != SQLITE_OK)
        database_error("Error adding new user: " + (std::string)error_message);
}

User *Users::get_user(const unsigned int &id)
{
    std::vector<Record> records =
        execute("SELECT * FROM users WHERE id=" + std::to_string(id));

    // Found no user with the id
    if (records.size() < 1)
        return nullptr;
    // Found more than 1 user with the same id. (Should be impossible)
    if (records.size() > 1)
        database_error("Found more than one user with id=" + std::to_string(id));
    else
    {
        Record &user_info = records[0];
        return new User(
            std::stoi(user_info[0]), user_info[1], Password::from_hash(user_info[2]),
            user_info[3], std::stod(user_info[4]), (Permissions)std::stoi(user_info[5]),
            [this](User &p) { this->save_changed_user(p); }
        );
    }
    return nullptr;
}

User *Users::get_user(const std::string &username)
{
    std::vector<Record> records =
        execute("SELECT * FROM users WHERE username=?", {username});

    // Found no user with the id
    if (records.size() < 1)
        return nullptr;
    // Found more than 1 user with the same id. (Should be impossible)
    if (records.size() > 1)
        database_error("Found more than one user with username=" + username);
    else
    {
        Record &user_info = records[0];
        return new User(
            std::stoi(user_info[0]), user_info[1], Password::from_hash(user_info[2]),
            user_info[3], std::stod(user_info[4]), (Permissions)std::stoi(user_info[5]),
            [this](User &p) { this->save_changed_user(p); }
        );
    }
    return nullptr;
}

const std::vector<User> &Users::list_users()
{
    std::vector<User> *users = new std::vector<User>();
    std::vector<Record> records = execute("SELECT * FROM users");
    for (unsigned int i = 0; i < records.size(); i++)
    {
        const Record &user_info = records[i];
        users->push_back(User(
            std::stoi(user_info[0]), user_info[1], Password::from_hash(user_info[2]),
            user_info[3], std::stod(user_info[4]), (Permissions)std::stoi(user_info[5]),
            [this](User &p) { this->save_changed_user(p); }
        ));
    }
    return *users;
}

const std::vector<User> &Users::get_users_by_permissions(const Permissions &permissions)
{
    std::vector<User> *users = new std::vector<User>();
    std::vector<Record> records =
        execute("SELECT * FROM users WHERE permissions=" + std::to_string(permissions));
    for (unsigned int i = 0; i < records.size(); i++)
    {
        const Record &user_info = records[i];
        users->push_back(User(
            std::stoi(user_info[0]), user_info[1], Password::from_hash(user_info[2]),
            user_info[3], std::stod(user_info[4]), (Permissions)std::stoi(user_info[5]),
            [this](User &p) { this->save_changed_user(p); }
        ));
    }
    return *users;
}

void Users::set_database_path(std::string path)
{
    this->open_db(path.c_str());
}

void Users::save_changed_user(User &user)
{
    execute(
        "UPDATE users SET username=?, password=?, name=?, balance=?, permissions=? "
        "WHERE id=?",
        {user.get_username(), user.password.to_string(), user.get_name(),
         std::to_string(user.get_balance()), std::to_string(user.get_permissions()),
         std::to_string(user.get_id())}
    );
}
