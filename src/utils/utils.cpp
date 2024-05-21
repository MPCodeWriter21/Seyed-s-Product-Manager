#include "utils.hpp"
#include <iostream>
#include <regex>

const char *PHONE_REGEX = "\\+(9[976]\\d|8[987530]\\d|6[987]\\d|5[90]\\d|42\\d|3[875]"
                          "\\d|2[98654321]\\d|9[8543210]|8[6421]|6[6543210]|5[87654321]"
                          "|4[987654310]|3[9643210]|2[70]|7|1)\\d{1,15}$";

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

bool validate_phone_number(const std::string &phone_number)
{
    if (phone_number.length() < 3 || phone_number.length() > 19)
        return false;
    if (std::regex_match(phone_number, std::regex(PHONE_REGEX)))
        return true;
    return false;
}
