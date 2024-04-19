#include <exception>
#include <iostream>
#include <string>

void error(std::string text)
{
    std::cerr << text << std::endl;
    throw std::exception();
}
