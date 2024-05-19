#include "argparse.hpp"
#include "exceptions.hpp"
#include <algorithm>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <ostream>
#include <string>
#include <unordered_map>
#include <vector>

// Arguments

Arguments::Arguments()
{
    n = 0;
}

template <typename T>
void Arguments::add_argument(const std::string &name, const T *value, const TYPE &type)
{
    arguments.push_back(Argument(name, (void *)value, type));
    n++;
}

void *Arguments::get(const std::string &name) const
{
    for (unsigned int i = 0; i < n; i++)
    {
        if (arguments[i].name == name)
        {
            return arguments[i].value;
        }
    }
    return nullptr;
}

// ArgumentParser

ArgumentParser::ArgumentParser(
    const std::string &name, const std::string &description, const bool &skip_help
)
{
    this->name = name;
    this->description = description;
    if (!skip_help)
        this->add_argument(
            {"--help", "-h"}, TYPE::NO_INPUT, "",
            [this]() {
                show_help();
                std::exit(0);
            },
            "Shows this help text."
        );
}

ArgumentParser &ArgumentParser::add_argument(
    const std::vector<std::string> flags,
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
    for (unsigned int i = 0; i < flags.size(); i++)
        for (unsigned int j = 0; j < arguments.size(); j++)
            for (unsigned int k = 0; k < arguments[j].flags.size(); k++)
                if (flags[i] == arguments[j].flags[k])
                    parser_error("Flag: `" + flags[i] + "` already exists.");
    // Check if destination_name already exists
    if (destination_name != "")
        for (unsigned int i = 0; i < arguments.size(); i++)
            if (destination_name == arguments[i].destination_name)
                parser_error(
                    "Destination Name: `" + destination_name + "` already exists."
                );
    arguments.push_back(
        _Argument(flags, type, destination_name, *new std::function(callback), help)
    );
    return *this;
}

ArgumentParser &ArgumentParser::add_argument(
    const std::initializer_list<std::string> flags,
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
            "You must specify `destination_name` for `" + flags.begin()[0] +
            "` since its not of type NO_INPUT."
        );
    }
    // Check if any of the flags already exist
    for (unsigned int i = 0; i < flags.size(); i++)
        for (unsigned int j = 0; j < arguments.size(); j++)
            for (unsigned int k = 0; k < arguments[j].flags.size(); k++)
                if (flags.begin()[i] == arguments[j].flags[k])
                    parser_error("Flag: `" + flags.begin()[i] + "` already exists.");
    // Check if destination_name already exists
    if (destination_name != "")
        for (unsigned int i = 0; i < arguments.size(); i++)
            if (destination_name == arguments[i].destination_name)
                parser_error(
                    "Destination Name: `" + destination_name + "` already exists."
                );
    arguments.push_back(
        _Argument(flags, type, destination_name, *new std::function(callback), help)
    );
    return *this;
}

Arguments ArgumentParser::parse_args(int argc, char *argv[])
{
    Arguments output_arguments;
    std::unordered_map<std::string, const _Argument *> flag_map;
    std::vector<const _Argument *> flagless_arguments;
    for (unsigned int i = 0; i < arguments.size(); i++)
    {
        if (arguments[i].flags.size() == 0)
            flagless_arguments.push_back(&arguments[i]);
        for (unsigned int j = 0; j < arguments[i].flags.size(); j++)
            flag_map[arguments[i].flags[j]] = &arguments[i];
    }
    filename = argv[0];
    for (int i = 1; i < argc; i++)
    {
        if (flag_map.contains(argv[i]))
        {
            const _Argument *arg = flag_map[argv[i]];
            if (arg->destination_name != "" && arg->type != TYPE::BOOLEAN_ARGUMENT)
            {
                if (i + 1 >= argc)
                    parser_error(
                        "Flag `" + std::string(argv[i]) + "` needs an argument!"
                    );

                std::string input_value(argv[++i]);
                void *value;
                switch (arg->type)
                {
                    case TYPE::INTEGER: {
                        int *x = new int(std::stoi(input_value));
                        value = (void *)x;
                        break;
                    }
                    case TYPE::DOUBLE_NUMBER: {
                        double *y = new double(std::stod(input_value));
                        value = (void *)y;
                        break;
                    }
                    default: {
                        std::string *z = new std::string(input_value);
                        value = (void *)z;
                    }
                }

                output_arguments.add_argument(arg->destination_name, value, arg->type);
            }
            else if (arg->destination_name != "" && arg->type == TYPE::BOOLEAN_ARGUMENT)
            {
                bool *x = new bool(true);
                output_arguments.add_argument(arg->destination_name, x, arg->type);
            }
            if (arg->callback != nullptr)
                arg->callback();
        }
        else
        {
            if (flagless_arguments.size() > 0)
            {
                const _Argument *arg = flagless_arguments[0];
                std::string input_value(argv[i]);
                void *value;
                switch (arg->type)
                {
                    case TYPE::INTEGER: {
                        int *x = new int(std::stoi(input_value));
                        value = (void *)x;
                        break;
                    }
                    case TYPE::DOUBLE_NUMBER: {
                        double *y = new double(std::stod(input_value));
                        value = (void *)y;
                        break;
                    }
                    default: {
                        std::string *z = new std::string(input_value);
                        value = (void *)z;
                    }
                }
                output_arguments.add_argument(arg->destination_name, value, arg->type);
                flagless_arguments.erase(flagless_arguments.begin());
            }
        }
    }

    // Make sure booleans are there
    for (unsigned int i = 0; i < arguments.size(); i++)
    {
        if (arguments[i].type == TYPE::BOOLEAN_ARGUMENT &&
            output_arguments.get(arguments[i].destination_name) == nullptr)
            output_arguments.add_argument(
                arguments[i].destination_name, new bool(false), TYPE::BOOLEAN_ARGUMENT
            );
    }
    return output_arguments;
}

void ArgumentParser::show_help() const
{
    std::cout << get_help_text() << std::endl;
}

const std::string ArgumentParser::get_help_text() const
{
    std::string help_text = "usage: " + filename;
    unsigned int usage_filename_length = help_text.size();
    std::vector<const _Argument *> positional_arguments;
    std::string dest = "";

    // Generate the usage statement
    for (unsigned int i = 0; i < arguments.size(); i++)
    {
        if (arguments[i].flags.size() == 0)
        {
            positional_arguments.push_back(&arguments[i]);
            continue;
        }
        help_text += " [" + arguments[i].flags[0];
        dest = arguments[i].destination_name;
        if (dest != "")
        {
            std::transform(dest.begin(), dest.end(), dest.begin(), ::toupper);
            help_text += " " + dest;
        }
        help_text += "]";
    }
    if (positional_arguments.size())
    {
        help_text += "\n";
        for (unsigned int _ = 0; _ < usage_filename_length; _++)
            help_text += " ";
    }
    for (unsigned int i = 0; i < positional_arguments.size(); i++)
    {
        help_text += " [" + positional_arguments[i]->destination_name + "]";
    }

    // Add positional arguments to the help text (if any)
    if (positional_arguments.size())
        help_text += "\n\nPositional Arguments:\n";
    for (unsigned int i = 0; i < positional_arguments.size(); i++)
    {
        help_text += "  " + positional_arguments[i]->destination_name + "\t\t" +
                     positional_arguments[i]->help + "\n";
    }
    if (help_text[help_text.size() - 1] != '\n')
        help_text += "\n";

    // Add flags to the help text
    help_text += "\nOptions:";
    for (unsigned int i = 0; i < arguments.size(); i++)
    {
        if (arguments[i].flags.size() == 0)
            continue;
        help_text += "\n  ";
        for (unsigned int j = 0; j < arguments[i].flags.size(); j++)
        {
            if (j > 0)
                help_text += ", ";
            help_text += arguments[i].flags[j];
            dest = arguments[i].destination_name;
            if (dest != "")
            {
                std::transform(dest.begin(), dest.end(), dest.begin(), ::toupper);
                help_text += " " + dest;
            }
        }
        if (arguments[i].flags.size() == 0)
        {
            dest = arguments[i].destination_name;
            std::transform(dest.begin(), dest.end(), dest.begin(), ::toupper);
            help_text += " " + dest;
        }
        if (arguments[i].help != "")
            help_text += "\t\t" + arguments[i].help;
    }
    return help_text + "\n";
}

void ArgumentParser::parser_error(const std::string &text) const
{
    error("Error: " + this->name + ": " + text);
}

ArgumentParser &ArgumentParser::add_subparser(
    const std::string &name, const std::string &description
)
{
    subparsers.push_back(ArgumentParser(name, description));
    return subparsers[subparsers.size() - 1];
}
