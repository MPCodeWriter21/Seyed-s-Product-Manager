#pragma once

#include <iomanip>
#include <string>

bool replace(std::string &str, const std::string &from, const std::string &to);

class comma_numpunct : public std::numpunct<char>
{
  protected:
    virtual char do_thousands_sep() const;

    virtual std::string do_grouping() const;
};

void enable_comma_locale();
