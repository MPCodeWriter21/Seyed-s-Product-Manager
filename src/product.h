#include <string>
#include <vector>

class Product
{
  public:
    Product(
        unsigned int id, std::string name, double price, std::string description = ""
    );

    const unsigned int &get_id() const;

  protected:
    unsigned int id;
    std::string name;
    double price;
    std::string description;
};

class Products
{
  public:
    Products();
    void add_product(const Product product);
    const Product *get_product(const unsigned int &id) const;
    const std::vector<Product> &list_products(const unsigned int &id) const;
    const bool save_database() const;
    const bool save_database(std::string path) const;
    void set_database_path(std::string path);
    static Products *load_from_database(std::string path);

  private:
    std::vector<Product> products;
    std::string path;
};
