#include "utils.hpp"
#include <iostream>

void replace(std::string &str, const std::string &from, const std::string &to)
{
    if (from == to)
        return;
    size_t start_pos, offset = 0;
    while ((start_pos = str.find(from, offset)) != std::string::npos)
    {
        str.replace(start_pos, from.length(), to);
        offset = start_pos + to.length();
    }
}

std::string escape_string(const std::string &text)
{
    std::string escaped(text);
    replace(escaped, "\\", "\\\\");
    replace(escaped, "\"", "\\\"");
    replace(escaped, "\'", "\\\'");
    return escaped;
}

std::string unescape_string(const std::string &escaped_text)
{
    std::string text = "";
    bool escaped = false;
    for (char c : escaped_text)
    {
        if (escaped)
        {
            text += c;
            escaped = false;
        }
        else if (c == '\\')
            escaped = true;
        else
            text += c;
    }
    return text;
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
