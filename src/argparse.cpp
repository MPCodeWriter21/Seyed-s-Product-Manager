#include "argparse.h"
#include <functional>
#include <iostream>
#include <ostream>
#include <stdexcept>
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
        throw std::invalid_argument(
            "Argument with type (NO_INPUT) must have a callback function."
        );
    }
    else if (type != TYPE::NO_INPUT && destination_name == "")
    {
        throw std::invalid_argument(
            "You must specify `destination_name` for `" + flags[0] +
            "` since its not of type NO_INPUT."
        );
    }
    // TODO: Check if any of the flags already exist
    // TODO: Check if destination_name already exists
    std::cout << flags[0] << std::endl;
    std::cout << &callback << std::endl;
    arguments.push_back(_Argument(
        flags, number_of_flags, type, destination_name, *new std::function(callback),
        help
    ));
    std::cout << &arguments[arguments.size() - 1].callback << std::endl;
    return *this;
}

Arguments ArgumentParser::parse_args(int argc, char *argv[]) const
{
    // Currently debugging this part!
    // 
    // --help
    // 000000FE1C9BF600
    // 000002BF85D326A0
    // -a
    // 000000FE1C9BF788
    // 000002BF85D34260
    // --help
    // -a
    // 000002BF85D34260
    // Hellooooo
    // Here we are!
    //
    Arguments output_arguments;
    std::unordered_map<std::string, _Argument *> flag_map;
    for (_Argument arg : arguments)
    {
        for (unsigned int i = 0; i < arg.number_of_flags; i++)
            flag_map[arg.flags[i]] = &arg;
    }
    for (int i = 0; i < argc; i++)
    {
        if (flag_map.contains(argv[i]))
        {
            std::cout << argv[i] << std::endl;
            _Argument *arg = flag_map[argv[i]];
            std::cout << arg->flags[0] << std::endl;
            if (arg->destination_name != "")
            {
                // TODO: Check if argv[i++] even exists
                output_arguments.add_argument(
                    arg->destination_name, argv[++i], arg->type
                );
            }
            std::cout << &arg->callback << std::endl;
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
