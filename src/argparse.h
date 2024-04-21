/*
 * A simple command-line argument parser inspired by Python's `argparse` module.
 */
#pragma once

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

enum TYPE
{
    // TODO: Add a type that represents the options that do not use flags
    NO_INPUT,
    BOOL,
    STRING,
    INT,
    DOUBLE
};

struct Argument
{
    std::string name;
    void *value;
    TYPE type;
};

class Arguments
{
  private:
    std::vector<Argument> arguments;
    unsigned int n;

  public:
    Arguments();
    template <typename T>
    void add_argument(const std::string &name, const T *value, const TYPE &type);
    void *get(const std::string &name) const;
};

struct _Argument
{
    const std::string *flags;
    unsigned int number_of_flags;
    TYPE type;
    std::string destination_name = "";
    const std::function<void()> &callback = nullptr;
    std::string help = "";
};

class ArgumentParser
{
  private:
    std::string name, description;
    std::vector<_Argument> arguments;

  public:
    ArgumentParser(
        const std::string &name = "Main parser", const std::string &description = ""
    );
    ArgumentParser &add_argument(
        const std::string flags[],
        const unsigned int number_of_flags,
        const TYPE &type,
        const std::string destination_name = "",
        const std::function<void()> &callback = nullptr,
        const std::string help = ""
    );
    Arguments parse_args(int argc, char *argv[]) const;
    const std::string get_help() const;
    const void parser_error(std::string text) const;
};
