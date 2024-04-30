#include <cstdlib>
#include <iostream>
#include <string>

void error(std::string text, bool exit = true)
{
    std::cerr << text << std::endl;
    std::exit(1);
}
