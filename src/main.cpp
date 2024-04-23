#include "argparse.h"
#include "product.h"
#include <iostream>
#include <string>
#include <vector>

int main(int argc, char *argv[])
{
    ArgumentParser parser("Main Parser", "Seyed's Product Manager");
    parser.add_argument(
        new std::string[]{}, 0, TYPE::STRING, "command", nullptr,
        "Commands: add-product, get-product, list-products, list-sold-out, edit-product"
    );
    parser.add_argument(
        new std::string[]{"-i", "--product-id"}, 2, TYPE::INT, "product-id", nullptr,
        "Product ID"
    );
    parser.add_argument(
        new std::string[]{"-n", "--product-name"}, 2, TYPE::STRING, "product-name",
        nullptr, "Product name"
    );
    parser.add_argument(
        new std::string[]{"-p", "--price"}, 2, TYPE::DOUBLE, "price", nullptr,
        "Price of the Product"
    );
    parser.add_argument(
        new std::string[]{"-a", "--available-count"}, 2, TYPE::INT, "available-count",
        nullptr, "Product ID"
    );
    parser.add_argument(
        new std::string[]{"-d", "--product-description"}, 2, TYPE::STRING,
        "description", nullptr, "Product Description"
    );
    auto args = parser.parse_args(argc, argv);
    Products products("./db.sqlite3");
    std::string *command = (std::string *)args.get("command");
    if (command == nullptr)
    {
        parser.show_help();
        parser.parser_error("Please enter a command!");
    }
    if (*command == "add-product")
    {
        int *id = (int *)args.get("product-id");
        std::string *name = (std::string *)args.get("product-name");
        double *price = (double *)args.get("price");
        int *available_count = (int *)args.get("available-count");
        std::string *description = (std::string *)args.get("description");
        // Checks to see if valid arguments were supplied to the program
        if (id != nullptr)
        {
            parser.show_help();
            parser.parser_error("ID is determined for the product automatically.");
        }
        if (name == nullptr)
        {
            parser.show_help();
            parser.parser_error("You need to specify a product-name to add a product.");
        }
        if (price == nullptr)
        {
            parser.show_help();
            parser.parser_error("You need to specify a price for the product.");
        }
        if (available_count == nullptr)
        {
            parser.show_help();
            parser.parser_error(
                "How many products do we have available? Please tell me."
            );
        }
        if (description == nullptr)
        {
            description = new std::string("");
        }
        std::cout << "Adding `" << *name << "`..." << std::endl;
        products.add_product(*name, *price, *available_count, *description);
    }
    else if (*command == "get-product")
    {
        int *id = (int *)args.get("product-id");
        std::string *name = (std::string *)args.get("product-name");
        double *price = (double *)args.get("price");
        int *available_count = (int *)args.get("available-count");
        std::string *description = (std::string *)args.get("description");
        // Checks to see if valid arguments were supplied to the program
        if (id == nullptr)
        {
            parser.show_help();
            parser.parser_error("You must specify the product's ID.");
        }
        // TODO: Add a way other than using product-id to get the product
        if (name != nullptr || price != nullptr || available_count != nullptr ||
            description != nullptr)
        {
            parser.show_help();
            parser.parser_error("You can only use product id to get the product info.");
        }
        std::cout << "Getting `" << *id << "`..." << std::endl;
        const Product *product = products.get_product(*id);
        if (product == nullptr)
            std::cerr << "Could not find a product with id=" + std::to_string(*id)
                      << std::endl;
        else
        {
            product->show_info();
        }
    }
    else if (*command == "list-products")
    {
        const std::vector<Product> &list_of_products = products.list_products();
        for (unsigned int i = 0; i < list_of_products.size(); i++)
        {
            list_of_products[i].show_info();
            std::cout << std::endl;
        }
    }
    else if (*command == "list-sold-out")
    {
        const std::vector<Product> &list_of_products = products.get_sold_out_products();
        for (unsigned int i = 0; i < list_of_products.size(); i++)
        {
            list_of_products[i].show_info();
            std::cout << std::endl;
        }
    }
    else if (*command == "edit-product")
    {
        int *id = (int *)args.get("product-id");
        std::string *name = (std::string *)args.get("product-name");
        double *price = (double *)args.get("price");
        int *available_count = (int *)args.get("available-count");
        std::string *description = (std::string *)args.get("description");
        // Checks to see if valid arguments were supplied to the program
        if (id == nullptr)
        {
            parser.show_help();
            parser.parser_error("You must specify the product's ID.");
        }
        if (name == nullptr && price == nullptr && available_count == nullptr &&
            description == nullptr)
        {
            parser.show_help();
            parser.parser_error(
                "What exactly are you planning to change about the product?"
            );
        }
        std::cout << "Modifying `" << *id << "`..." << std::endl;
        Product *product = products.get_product(*id);
        if (product == nullptr)
            std::cerr << "Could not find a product with id=" + std::to_string(*id)
                      << std::endl;
        else
        {
            if (name != nullptr)
                product->set_name(*name);
            if (price != nullptr)
                product->set_price(*price);
            if (available_count != nullptr)
                product->set_available_count(*available_count);
            if (description != nullptr)
                product->set_description(*description);
        }
    }
    else
        parser.parser_error(
            "You must use one of the command that are available! Check --help."
        );
    return 0;
}
