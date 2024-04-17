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
    parser.parse_args(argc, argv);
    std::cout << "Here we are!\n";
    return 0;
}
