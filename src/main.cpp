#include <iostream>

int main(int argc, char *argv[])
{
    while (--argc)
        std::cout << *(++argv) << "\n";
    return 0;
}
