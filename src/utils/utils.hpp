#pragma once

#include <iomanip>
#include <string>

void replace(std::string &str, const std::string &from, const std::string &to);

std::string escape_string(const std::string &text);
std::string unescape_string(const std::string &escaped_text);

class comma_numpunct : public std::numpunct<char>
{
  protected:
    virtual char do_thousands_sep() const;

    virtual std::string do_grouping() const;
};

void enable_comma_locale();

bool validate_phone_number(const std::string &phone_number);
