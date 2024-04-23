#include "argparse.h"
#include "product.h"
#include <iostream>
#include <string>

int main(int argc, char *argv[])
{
    ArgumentParser parser("Main Parser", "Seyed's Product Manager");
    parser.add_argument(
        new std::string[]{}, 0, TYPE::STRING, "command",
        nullptr, "Some command"
    );
    parser.add_argument(
        new std::string[]{}, 0, TYPE::STRING, "command2",
        nullptr, "Some command"
    );
    parser.add_argument(
        new std::string[]{"-a", "--add-product"}, 2, TYPE::STRING, "product_to_add",
        nullptr, "Product name"
    );
    parser.add_argument(
        new std::string[]{"-g", "--get-product"}, 2, TYPE::INT, "product_to_get",
        nullptr, "Product id"
    );
    auto args = parser.parse_args(argc, argv);
    Products products("./db.sqlite3");
    std::string *command = (std::string *)args.get("command");
    std::cout << *command << std::endl;
    std::string *command2 = (std::string *)args.get("command2");
    std::cout << *command2 << std::endl;
    std::string *product_to_add = (std::string *)args.get("product_to_add");
    int *product_to_get = (int *)args.get("product_to_get");
    if (product_to_add != nullptr)
    {
        std::cout << "Adding `" << *product_to_add << "`..." << std::endl;
        products.add_product(*product_to_add, 1000, 1, "Some new product.");
    }
    else if (product_to_get != nullptr)
    {
        std::cout << "Getting `" << *product_to_get << "`..." << std::endl;
        const Product *product = products.get_product(*product_to_get);
        if (product == nullptr)
            std::cerr << "Could not find a product with id=" +
                             std::to_string(*product_to_get) << std::endl;
        else
        {
            std::cout << "Product ID               : " << product->get_id()
                      << std::endl;
            std::cout << "Name                     : " << product->get_name()
                      << std::endl;
            std::cout << "Price                    : " << product->get_price()
                      << std::endl;
            std::cout << "No. of Available Products: " << product->get_available_count()
                      << std::endl;
            std::cout << "Product Descriptions     : " << product->get_id()
                      << std::endl;
        }
    }
    else
        parser.parser_error("You must either add or get a product! (Use -a or -g)");
    std::cout << "Done!\n";
    return 0;
}
