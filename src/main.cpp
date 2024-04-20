#include "argparse.h"
#include <iostream>
#include <string>

int main(int argc, char *argv[])
{
    ArgumentParser parser("Main Parser", "Seyed's Product Manager");
    parser.add_argument(
        new std::string[]{"-a", "--first-n"}, 2, TYPE::NO_INPUT, "",
        []() { std::cout << "Hellooooo\n"; }, "The First Number"
    );
    parser.add_argument(new std::string[]{"-b"}, 1, TYPE::INT, "b", nullptr, "b");
    parser.add_argument(
        new std::string[]{"-c"}, 1, TYPE::DOUBLE, "c", nullptr, "c is a double!"
    );
    parser.add_argument(
        new std::string[]{"-v", "--verbose"}, 2, TYPE::BOOL, "verbose", nullptr,
        "Use this to get more info."
    );
    auto args = parser.parse_args(argc, argv);
    std::cout << "Here we are!\n";
    int *x = (int *)args.get("b");
    if (x != nullptr)
        std::cout << *x << std::endl;
    double *c = (double *)args.get("c");
    if (c != nullptr)
        std::cout << *c << std::endl;
    if (args.get("verbose"))
    {
        if (x != nullptr)
            std::cout << "You used flag '-b'!\n";
        if (c != nullptr)
            std::cout << "You used flag '-c'!\n";
    }
    return 0;
}
