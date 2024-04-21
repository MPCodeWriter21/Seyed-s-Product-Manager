#include "argparse.h"
#include "product.h"
#include <iostream>
#include <string>

int main(int argc, char *argv[])
{
    ArgumentParser parser("Main Parser", "Seyed's Product Manager");
    parser.add_argument(
        new std::string[]{"-a", "--add-product"}, 2, TYPE::STRING, "product", nullptr,
        "Product name"
    );
    auto args = parser.parse_args(argc, argv);
    std::string *product_name = (std::string *)args.get("product");
    if (product_name == nullptr)
        parser.parser_error("You must specify the product name. (Use -a)");
    std::cout << "Product name is: " << *product_name << std::endl;
    Products products("./db.sqlite3");
    products.add_product(*product_name, 1000, 1, "Some new product.");
    std::cout << "Done!\n";
    return 0;
}
