#include "database/order.hpp"
#include "database/product.hpp"
#include "database/user.hpp"
#include "gui/gui.hpp"
#include "utils/argparse.hpp"
#include "utils/utils.hpp"
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

int main(int argc, char *argv[])
{
    ArgumentParser parser("Main Parser", "Seyed's Product Manager");
    parser.add_argument(
        {}, TYPE::STRING, "command", nullptr,
        "Commands: add-product, get-product, list-products, list-sold-out, "
        "edit-product, add-user, new-order, list-orders, get-order, pay-order, turnover"
    );
    parser.add_argument(
        {"-i", "--product-id"}, TYPE::INTEGER, "product-id", nullptr, "Product ID"
    );
    parser.add_argument(
        {"-n", "--product-name"}, TYPE::STRING, "product-name", nullptr, "Product name"
    );
    parser.add_argument(
        {"-p", "--price"}, TYPE::DOUBLE_NUMBER, "price", nullptr, "Price of the Product"
    );
    parser.add_argument(
        {"-a", "--available-count"}, TYPE::INTEGER, "available-count", nullptr,
        "Product ID"
    );
    parser.add_argument(
        {"-d", "--product-description"}, TYPE::STRING, "description", nullptr,
        "Product Description"
    );
    parser.add_argument(
        {"-I", "--order-id"}, TYPE::INTEGER, "order-id", nullptr, "Order ID"
    );
    parser.add_argument(
        {"-f", "--from-date"}, TYPE::STRING, "from-date", nullptr,
        "Starting date for the financial turnover report"
    );
    parser.add_argument(
        {"-t", "--to-date"}, TYPE::STRING, "to-date", nullptr,
        "Ending date for the financial turnover report"
    );
    auto args = parser.parse_args(argc, argv);

    enable_comma_locale();

    // Init users' database
    // TODO: Make it possible for Seyed to use a custom password instead of the random
    // one that the app generates
    std::string *password = new std::string("");
    Users users("./db.sqlite3", "Seyed", password);
    if (*password != "")
        std::cout << "Root User:\n\tUsername: Seyed\n\tPassword: " << *password
                  << "\n\n\n";

    // Products' database
    // NOTE: Bad practice (But I don't care - it lets me use fewer number of files)
    Products products((Products &)users);
    Orders orders((Orders &)users);

    // Get the command from the parser and check what options is chosen by the user
    std::string *command = (std::string *)args.get("command");
    if (command == nullptr)
    {
        return run_gui(users);
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
        delete description;
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
    else if (*command == "add-user")
    {
        // TODO: ...
        std::cout << "Not implemented yet..." << std::endl;
    }
    else if (*command == "new-order")
    {
        std::vector<ProductOrder> product_orders;
        unsigned int product_id, count;
        while (true)
        {
            std::cout << "Enter the product id (or 0 to stop): ";
            std::cin >> product_id;
            if (product_id == 0)
                break;
            Product *product = products.get_product(product_id);
            if (product == nullptr)
            {
                std::cout << "Product with id " << product_id << " does not exist!\n";
                continue;
            }
            int product_order_index = -1;
            for (size_t i = 0; i < product_orders.size(); i++)
            {
                if (product_orders[i].product.get_id() == product->get_id())
                {
                    product_order_index = i;
                    break;
                }
            }
            if (product_order_index == -1)
                std::cout << "Enter the number of products: ";
            else
                std::cout << "Enter the number of products(Currently "
                          << product_orders[product_order_index].count << "): ";
            std::cin >> count;
            if (count < 1)
            {
                std::cout << "No product was added to the order!\n";
                continue;
            }
            if (count > product->get_available_count())
            {
                std::cout << "Sorry but there are only "
                          << product->get_available_count()
                          << " of this product left in stock!\n";
                continue;
            }
            if (product_order_index == -1)
                product_orders.push_back(ProductOrder(*product, count));
            else
                product_orders[product_order_index].count = count;
        }
        if (product_orders.size() < 1)
        {
            std::cout << "No product means no order!\n";
            return 0;
        }
        int discount = 0;
        std::cout << "Any discounts? If not just enter 0: ";
        while (true)
        {
            std::cin >> discount;
            if (discount < 0 || discount > 100)
            {
                std::cout << "Sorry but it's not possible to have a " << discount
                          << "% discount!" << std::endl
                          << "Maybe try again? Discount(0-100): ";
            }
            else
                break;
        }
        std::cout << "Creating a new order..." << std::endl;
        orders.add_order(product_orders, discount);
    }
    else if (*command == "list-orders")
    {
        const std::vector<Order> &list_of_orders = orders.list_orders(products);
        for (unsigned int i = 0; i < list_of_orders.size(); i++)
        {
            list_of_orders[i].show_info();
            std::cout << std::endl;
        }
    }
    else if (*command == "get-order")
    {
        int *id = (int *)args.get("order-id");
        // Checks to see if valid arguments were supplied to the program
        if (id == nullptr)
        {
            parser.show_help();
            parser.parser_error("You must specify the order's ID.");
        }
        std::cout << "Getting order with id `" << *id << "`..." << std::endl;
        const Order *order = orders.get_order(*id, products);
        if (order == nullptr)
        {
            std::cerr << "Could not find an order with id=" + std::to_string(*id)
                      << std::endl;
            return 0;
        }
        order->show_info();
        std::vector<ProductOrder> product_orders = order->get_product_orders();
        for (unsigned int i = 0; i < order->get_count(); i++)
        {
            std::cout << "\tProduct order " << i + 1 << ":\n";
            std::cout << "\tProduct ID     : " << product_orders[i].product.get_id()
                      << std::endl;
            std::cout << "\tProduct Name   : " << product_orders[i].product.get_name()
                      << std::endl;
            std::cout << "\tPrice of one   : " << std::setprecision(2) << std::fixed
                      << product_orders[i].product.get_price() << std::endl;
            std::cout << "\tCount          : " << product_orders[i].count << std::endl;
            std::cout << "\tTotal amount   : " << std::setprecision(2) << std::fixed
                      << product_orders[i].count * product_orders[i].product.get_price()
                      << std::endl;
            std::cout << std::endl;
        }
    }
    else if (*command == "pay-order")
    {
        int *id = (int *)args.get("order-id");
        // Checks to see if valid arguments were supplied to the program
        if (id == nullptr)
        {
            parser.show_help();
            parser.parser_error("You must specify the order's ID.");
        }
        Order *order = orders.get_order(*id, products);
        if (order == nullptr)
        {
            std::cerr << "Could not find an order with id=" + std::to_string(*id)
                      << std::endl;
            return 0;
        }
        if (order->is_paid())
        {
            std::cerr << "Order with id=" + std::to_string(*id) + " is already paid!\n";
            return 0;
        }
        const std::vector<ProductOrder> &product_orders = order->get_product_orders();
        for (size_t i = 0; i < product_orders.size(); i++)
        {
            if (product_orders[i].count >
                product_orders[i].product.get_available_count())
            {
                std::cerr << "Sorry but there are only "
                          << product_orders[i].product.get_available_count() << " of "
                          << product_orders[i].product.get_name()
                          << " is left and we cannot sell " << product_orders[i].count
                          << " of them!" << std::endl;
                return 0;
            }
        }
        if (order->pay())
            std::cout << "The order with id=" << std::to_string(*id)
                      << " was paid successfully!\n";
        else
            std::cout << "There was a problem paying the order with id="
                      << std::to_string(*id) << std::endl;
    }
    else if (*command == "turnover")
    {
        // Show the financial turnover
        std::string *from_date = (std::string *)args.get("from-date");
        std::string *to_date = (std::string *)args.get("to-date");
        std::vector<Order> list_of_orders;
        if (from_date == nullptr && to_date == nullptr)
            list_of_orders = orders.get_orders({"now", "-1 month"}, {}, products);
        else if (from_date != nullptr && to_date == nullptr)
            list_of_orders = orders.get_orders({*from_date}, {}, products);
        else if (from_date == nullptr && to_date != nullptr)
            list_of_orders =
                orders.get_orders({"now", "-1 month"}, {*to_date}, products);
        else
            list_of_orders = orders.get_orders({*from_date}, {*to_date}, products);
        if (list_of_orders.size() < 1)
        {
            std::cout << "We did not get any payments in that time frame :(\n";
            return 0;
        }
        std::cout << "We got " << list_of_orders.size() << " payments!" << std::endl;
        double total_profit = 0;
        double total_no_discount_sales = 0;
        for (size_t i = 0; i < list_of_orders.size(); i++)
        {
            total_no_discount_sales += list_of_orders[i].get_total();
            total_profit += list_of_orders[i].get_total_after_discount();
        }
        std::cout << "We earned " << std::setprecision(2) << std::fixed << total_profit;
        if (total_profit != total_no_discount_sales)
            std::cout << " but we could have earned " << std::setprecision(2)
                      << std::fixed << total_no_discount_sales
                      << " if you hadn't given the customers so much discounts";
        std::cout << "!\n";
    }
    else
        parser.parser_error(
            "You must use one of the command that are available! Check --help."
        );
    return 0;
}
