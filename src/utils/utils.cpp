#include <iostream>
#include "utils.hpp"

bool replace(std::string &str, const std::string &from, const std::string &to)
{
    size_t start_pos = str.find(from);
    if (start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}

char comma_numpunct::do_thousands_sep() const
{
    return ',';
}

std::string comma_numpunct::do_grouping() const
{
    return "\03";
}

void enable_comma_locale()
{
    static std::locale comma_locale(std::locale(), new comma_numpunct());
    std::cout.imbue(comma_locale);
}
