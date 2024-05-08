/*
 * A simple command-line argument parser inspired by Python's `argparse` module.
 */
#pragma once

#include <functional>
#include <string>
#include <vector>

enum TYPE
{
    NO_INPUT,
    BOOLEAN_ARGUMENT,
    STRING,
    INTEGER,
    DOUBLE_NUMBER
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
  public:
    ArgumentParser(
        const std::string &name = "Main parser", const std::string &description = ""
    );
    // TODO: Add a way to make the arguments required
    ArgumentParser &add_argument(
        const std::string flags[],
        const unsigned int number_of_flags,
        const TYPE &type,
        const std::string destination_name = "",
        const std::function<void()> &callback = nullptr,
        const std::string help = ""
    );
    // TODO: Make it so that extra arguments would raise an error saying the arguments
    // are not recognized
    Arguments parse_args(int argc, char *argv[]);
    void show_help() const;
    const std::string get_help_text() const;
    void parser_error(const std::string &text) const;
    // TODO: Implement a logic that actually uses the subparsers
    ArgumentParser &add_subparser(
        const std::string &name, const std::string &description
    );

    // Whether it's required to use at least one subparser(If at least one is available)
    // If false, the user can pass arguments without using any subparser
    bool subparsers_required = true;

  private:
    std::string name, description;
    std::vector<_Argument> arguments;
    std::string filename;
    std::vector<ArgumentParser> subparsers;
};
