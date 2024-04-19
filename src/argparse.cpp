#include "argparse.h"
#include "exceptions.h"
#include <functional>
#include <iostream>
#include <ostream>
#include <string>
#include <unordered_map>

template <typename T>
void Arguments::add_argument(const std::string &name, const T *value, const TYPE &type)
{
    arguments.push_back(Argument(name, (void *)value, type));
}

template <typename T> T Arguments::operator[](const std::string &name) const
{
    for (unsigned int i = 0; i < n; i++)
    {
        if (arguments[i].name == name)
        {
            switch (arguments[i].type)
            {
            case TYPE::INT:
                return *((int *)arguments[i].value);
            case TYPE::DOUBLE:
                return *((double *)arguments[i].value);
            default:
                return *((std::string *)arguments[i].value);
            }
        }
    }
}

ArgumentParser::ArgumentParser(const std::string &name, const std::string &description)
{
    this->name = name;
    this->description = description;
    this->add_argument(
        new std::string[]{"--help", "-h"}, 2, TYPE::NO_INPUT, "",
        [this]() { std::cout << get_help() << std::endl; }, "Shows this help text."
    );
}

ArgumentParser &ArgumentParser::add_argument(
    const std::string flags[],
    const unsigned int number_of_flags,
    const TYPE &type,
    const std::string destination_name,
    const std::function<void()> &callback,
    const std::string help
)
{
    if (type == TYPE::NO_INPUT && callback == nullptr)
    {
        parser_error("Argument with type (NO_INPUT) must have a callback function.");
    }
    else if (type != TYPE::NO_INPUT && destination_name == "")
    {
        parser_error(
            "You must specify `destination_name` for `" + flags[0] +
            "` since its not of type NO_INPUT."
        );
    }
    // Check if any of the flags already exist
    for (unsigned int i = 0; i < number_of_flags; i++)
        for (unsigned int j = 0; j < arguments.size(); j++)
            for (unsigned int k = 0; k < arguments[j].number_of_flags; k++)
                if (flags[i] == arguments[j].flags[k])
                    parser_error("Flag: `" + flags[i] + "` already exists.");
    // Check if destination_name already exists
    if (destination_name != "")
        for (unsigned int i = 0; i < arguments.size(); i++)
            if (destination_name == arguments[i].destination_name)
                parser_error(
                    "Destination Name: `" + destination_name + "` already exists."
                );
    arguments.push_back(_Argument(
        flags, number_of_flags, type, destination_name, *new std::function(callback),
        help
    ));
    return *this;
}

Arguments ArgumentParser::parse_args(int argc, char *argv[]) const
{
    Arguments output_arguments;
    std::unordered_map<std::string, const _Argument *> flag_map;
    for (unsigned int i = 0; i < arguments.size(); i++)
    {
        for (unsigned int j = 0; j < arguments[i].number_of_flags; j++)
            flag_map[arguments[i].flags[j]] = &arguments[i];
    }
    for (int i = 0; i < argc; i++)
    {
        if (flag_map.contains(argv[i]))
        {
            const _Argument *arg = flag_map[argv[i]];
            if (arg->destination_name != "")
            {
                if (i + 1 >= argc)
                    parser_error(
                        "Flag `" + std::string(argv[i]) + "` needs an argument!"
                    );
                output_arguments.add_argument(
                    arg->destination_name, argv[++i], arg->type
                );
            }
            if (arg->callback != nullptr)
                arg->callback();
        }
    }
    return output_arguments;
}

const std::string ArgumentParser::get_help() const
{
    return "Help text";
}

const void ArgumentParser::parser_error(std::string text) const
{
    error("Error: " + this->name + ": " + text);
}
