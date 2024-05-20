Seyed's-Product-Manager
=======================
A simple store management project for Seyed written in C++.

> [!IMPORTANT]
> This is just a C++ exercise and is not intended to be used in real projects or referenced in other documents by any means.

Build
-----

### Requirements

+ [LLVM](https://clang.llvm.org/): We use clang to compile the project by default but
  feel free to change it to GCC or any other compiler if you feel like it.
+ [make](https://www.gnu.org/software/make/): If you look carefully you see that we have a `Makefile`.
  So... You know what you need.
+ The ability to now how to run commands in a command-line: Self explanatory

### Run the make command

You can build the project in `debug` or `release` mode:

```bash
# Build in release mode
make all

# Build in debug mode
make debug

# Build with no GUI
make all BUILD_GUI=no
```

> [!NOTE]
> Make sure to change the `ARCH` in the `Makefile` based on your system's architecture.

Features & Plans
----------------

+ [x] A CLI to easily interact with the application
+ [x] GUI (_In progress..._)
+ [ ] TUI
+ [ ] Web UI
+ [x] Stores product data, user data and orders in a SQLite database
+ [ ] A useful and feature heavy user management solution and hashing passwords
+ [x] A proper codebase, easy to build. Please make sure your environment is set up properly!
+ [ ] Colorful messages

Dependencies
------------

Currently other than build tools and a proper environment to run the project, it doesn't
have any dependencies.

Usage
-----

> [!WARN]
> If you are reading this in a branch other than the `master`, It's very possible that
> I have changed some stuff in the code and the examples will not work as expected.

### First time running:

When you run the program for the first time with no arguments submitted it will create
a new file named `db.sqlite3` in your working directory (which will be used to store 
program's data); and you will see some message like:
```
Root User:
        Username: Seyed
        Password: [RANDOM GENERATED PASSWORD]


usage: build\seyed_app.out [--help] [-i PRODUCT-ID] [-n PRODUCT-NAME] [-p PRICE] [-a AVAILABLE-COUNT] [-d DESCRIPTION] [-I ORDER-ID] [-f FROM-DATE] [-t TO-DATE]
       [command]

Positional Arguments:
  command               Commands: add-product, get-product, list-products, list-sold-out, ed
it-product, add-user, new-order, list-orders, get-order, pay-order, turnover

Options:
  --help, -h            Shows this help text.
  -i PRODUCT-ID, --product-id PRODUCT-ID                Product ID
  -n PRODUCT-NAME, --product-name PRODUCT-NAME          Product name
  -p PRICE, --price PRICE               Price of the Product
  -a AVAILABLE-COUNT, --available-count AVAILABLE-COUNT         Product ID
  -d DESCRIPTION, --product-description DESCRIPTION             Product Description
  -I ORDER-ID, --order-id ORDER-ID              Order ID
  -f FROM-DATE, --from-date FROM-DATE           Starting date for the financial turnover report
  -t TO-DATE, --to-date TO-DATE         Ending date for the financial turnover report
  --gui GUI             Run the GUI application

Error: Main Parser: Please enter a command!
```


Please save the random password safely! (Or don't? cuz currently it's used for nothing
and the database file is not password protected or encrypted in any way and the password
is saved as plain text in there... So... Yeah, my bad.)

### Reading the help message

Well; I tried to make the help message be pretty clear and standard so if you don't know
how to read it, it's probably because you've never read a help message before(I'm
saying it's your fault).

BTW, the help message is dynamically generated, you could take a look at the code at
[`src/utils/argparse.cpp`](./src/utils/argparse.cpp).

### Using commands and flags

Even though I said the help text is readable I never said that I included enough
information for you to understand what flags to use with each command and how everything
is done. So I will give you some examples and explanation here.

Firstly if you use a command and not supply needed arguments, the program will tell you
to give it the missing piece of information so don't worry if I didn't cover everything
here. (It is soooo smart that sometimes if you provide too many flags it will tell you
not to XD)

### Product stuff

+ `add-product`: Use this command with `-n` for name, `-p` for price and `-a` for
  specifying the number of products that are available. You can optionally use `-d` to
  provide some description for the product as well.

  Note: You can not set a custom `id` for the product, it will be generated automatically.
```bash
build/seyed_app.out add-product --product-name "My Product" --price 2121 --available-count 21 --product-description "In case you couldn't tell, I really like 21 8D"
```
+ `get-product`: Use this command to get info about a specific product by using its
  `id` (_Hopefully more ways will be added later_).
```bash
build/seyed_app.out get-product --product-id 8
```
+ `list-products`: I think the command is self-explanatory but in case you did not
  understand, it prints a list of products to the console.
```bash
build/seyed_app.out list-products
```
+ `list-sold-out`: It lists all sold out products.
```bash
build/seyed_app.out list-sold-out
```
+ `edit-product`: Use this command to edit a product's info.
```bash
# Rename the product with id=8 and set its name to `Product8` and its price to `9908`
build/seyed_app.out edit-product -i 8 -n Product8 -p 9907
```

### Order stuff

+ `new-order`: Use this command to create a new order. You won't have to supply any
  command-line arguments, the program will ask you about the order after you ran the
  command.
```bash
build/seyed_app.out new-order
```
+ `list-orders`: List all orders.
```bash
build/seyed_app.out list-orders
```
+ `get-order`: Shows all the information about an order, you must specify the order's
  `id` using `-I` flag.
```bash
# Get the information about the order with id=3
build/seyed_app.out get-order -I 3
```
+ `turnover`: Shows information about the financial turnover in a given time frame. (
  Default time frame: From 1 month ago to now)
```bash
# Running with no paramenters
build/seyed_app.out turnover

# Running with a specific starting and ending date
build/seyed_app.out turnover -f "2024-01-01" -t "2024-07-01"
```
  
Contributions
-------------

You're welcome to fork the project and change it in whatever way you like. Just remember
to format your code according to the `.clang-format` file before you make the
pull request with enough explanation for me to understand what you did.

Authors
-------

Main contributor:

+ *Mehrad Pooryoussof*: [CodeWriter21](https://GitHub.com/MPCodeWriter21/)
+ *Mahan Ghadimkhani*: [Mahan-Ghk](https://github.com/Mahan-Ghk)

License
-------

The [license](./LICENSE) is MIT and you can pretty much do what you want with the code and don't
bother me. Thanks!
