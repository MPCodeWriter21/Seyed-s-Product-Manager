#include <cstdlib>
#include <iostream>
#include <string>

void error(std::string text, bool exit = true)
{
    std::cerr << text << std::endl;
    if (exit)
        std::exit(1);
}
