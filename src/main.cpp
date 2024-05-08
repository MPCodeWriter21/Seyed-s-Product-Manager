#include "database/order.hpp"
#include "database/product.hpp"
#include "database/user.hpp"
#include "utils/argparse.hpp"
#include "utils/utils.hpp"
#include <iomanip>
#include <iostream>
#include <string>

// # gui
#include "gui/imgui.h"
#include "gui/imgui_impl_dx9.h"
#include "gui/imgui_impl_win32.h"
#include <d3d9.h>
#include <tchar.h>
#include <vector>

// Data
static LPDIRECT3D9 g_pD3D = nullptr;
static LPDIRECT3DDEVICE9 g_pd3dDevice = nullptr;
static UINT g_ResizeWidth = 0, g_ResizeHeight = 0;
static D3DPRESENT_PARAMETERS g_d3dpp = {};

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void ResetDevice();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int run_gui();

// Font addresses
extern char font_file_start[] asm("_binary_misc_Lalezar_Regular_ttf_start");
extern char font_file_end[] asm("_binary_misc_Lalezar_Regular_ttf_end");

int main(int argc, char *argv[])
{
    ArgumentParser parser("Main Parser", "Seyed's Product Manager");
    parser.add_argument(
        new std::string[]{}, 0, TYPE::STRING, "command", nullptr,
        "Commands: add-product, get-product, list-products, list-sold-out, "
        "edit-product, add-user, new-order, list-orders, get-order, pay-order"
    );
    parser.add_argument(
        new std::string[]{"-i", "--product-id"}, 2, TYPE::INTEGER, "product-id", nullptr,
        "Product ID"
    );
    parser.add_argument(
        new std::string[]{"-n", "--product-name"}, 2, TYPE::STRING, "product-name",
        nullptr, "Product name"
    );
    parser.add_argument(
        new std::string[]{"-p", "--price"}, 2, TYPE::DOUBLE_NUMBER, "price", nullptr,
        "Price of the Product"
    );
    parser.add_argument(
        new std::string[]{"-a", "--available-count"}, 2, TYPE::INTEGER, "available-count",
        nullptr, "Product ID"
    );
    parser.add_argument(
        new std::string[]{"-d", "--product-description"}, 2, TYPE::STRING,
        "description", nullptr, "Product Description"
    );
    parser.add_argument(
        new std::string[]{"-I", "--order-id"}, 2, TYPE::INTEGER, "order-id", nullptr,
        "Order ID"
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
        return run_gui();
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
        std::cout << "Creating a new order..." << std::endl;
        orders.add_order(product_orders);
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
    else
        parser.parser_error(
            "You must use one of the command that are available! Check --help."
        );
    return 0;
}

int run_gui()
{
    // Create application window
    // ImGui_ImplWin32_EnableDpiAwareness();
    WNDCLASSEXW wc = {
        sizeof(wc),
        CS_CLASSDC,
        WndProc,
        0L,
        0L,
        GetModuleHandle(nullptr),
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        L"ImGui Example",
        nullptr
    };
    ::RegisterClassExW(&wc);
    HWND hwnd = ::CreateWindowW(
        wc.lpszClassName, L"SEYED SHOP", WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800,
        nullptr, nullptr, wc.hInstance, nullptr
    );

    // Initialize Direct3D
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    // Show the window
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX9_Init(g_pd3dDevice);

    // Load Fonts
    char *binary_start = font_file_start;
    char *binary_end = font_file_end;
    io.Fonts->AddFontDefault();
    ImFont *mainfont =
        io.Fonts->AddFontFromMemoryTTF(binary_start, binary_end - binary_start, 30);
    IM_ASSERT(mainfont != NULL);
    // Our state
    // ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    ImVec4 clear_color = ImVec4(40, 40, 40, 50);
    // ImVec4 clear_color = ImVec4(255, 255, 255, 200);

    // Main loop
    bool done = false;
    bool dokme_sabt_mahsool = false;
    int gheimat = 0;
    bool dokme_Sabt_sefaresh = false;
    bool dokme_Sefareshat = false;
    bool dokme_Gardesh_mali = false;
    bool dokme_Mahsoolat = false;
    bool dokme_bastan = true;

    bool logged = false;
    std::string password = "1111";
    static char passput[128];
    bool wrong_pass = false;
    bool login_pass = false;

    ImGuiStyle &style = ImGui::GetStyle();

    // Blonge Style
    style.Alpha = 1.0f;
    style.FrameRounding = 7.0f;
    style.Colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    // style.Colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.22f, 0.35f, 0.70f);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
    style.Colors[ImGuiCol_PopupBg] = ImVec4(.00f, 1.00f, 1.00f, 0.94f);
    // style.Colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 0.20f);
    style.Colors[ImGuiCol_Border] = ImVec4(1.00f, 1.00f, 1.00f, 0.10f);
    style.Colors[ImGuiCol_BorderShadow] = ImVec4(1.00f, 1.00f, 1.00f, 0.10f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.05f, 0.00f, 0.20f, 0.94f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(1.00f, 0.45f, 0.12f, 1.00f);
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.45f, 0.12f, 0.70f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.90f, 0.35f, 0.02f, 1.00f);
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.98f, 0.98f, 0.98f, 0.53f);
    style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.69f, 0.69f, 0.69f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.59f, 0.59f, 0.59f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_Button] = ImVec4(0.15f, 0.02f, 0.68f, 1.00f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.50f);
    style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    style.Colors[ImGuiCol_PlotLines] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
    style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    // style.Colors[ImGuiCol_] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    style.WindowRounding = 10;
    style.WindowBorderSize = 2;
    // style.rounding
    // style.Colors[ImGuiCol_Button] = ImVec4(28, 41, 172, 50);
    // style.Colors[ImGuiCol_Border] = ImVec4(255, 255, 255, 0);
    while (!done)
    {
        // Poll and handle messages (inputs, window resize, etc.)
        // See the WndProc() function below for our to dispatch events to
        // the Win32 backend.
        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done)
            break;

        // Handle window resize (we don't resize directly in the WM_SIZE
        // handler)
        if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
        {
            g_d3dpp.BackBufferWidth = g_ResizeWidth;
            g_d3dpp.BackBufferHeight = g_ResizeHeight;
            g_ResizeWidth = g_ResizeHeight = 0;
            ResetDevice();
        }

        // Start the Dear ImGui frame
        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        // ImGui :: SetWindowPos  ( ImGui :: GetMainViewport () -> Pos ) ;
        // ImGui :: SetWindowSize ( ImGui :: GetMainViewport () -> Size ) ;

        // panjare man
        ImGui::PushFont(mainfont);
        ImGui::SetNextWindowPos(ImVec2(600, 250));
        ImGui::SetNextWindowSize(ImVec2(200, 200));
        if (login_pass == false)
        {
            if (ImGui::Begin(
                    "LOGIN", NULL,
                    ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                        ImGuiWindowFlags_NoMove
                ))
            {

                ImGui::Text("Enter Your Password");
                ImGui::InputText(
                    "##", passput, sizeof(passput),
                    ImGuiInputTextFlags_Password | ImGuiInputTextFlags_EnterReturnsTrue
                );
                if (wrong_pass == true)
                {
                    ImGui::Text("Wrong Password");
                }
                if (ImGui::Button("login"))
                {
                    if (passput == password)
                    {
                        logged = true;
                    }
                    else
                    {
                        wrong_pass = true;
                    }
                }
            }
            ImGui::End();
        }
        ImGui::SetCursorPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(600, 400));
        ImGui::SetNextWindowPos(ImVec2(400, 100));
        if (logged)
        {
            login_pass = true;
            if (ImGui::Begin(
                    "main panjare", NULL,
                    ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                        ImGuiWindowFlags_NoMove
                ))
            {
                // ImGui::SetWindowFontScale(2.0f);
                ImGui::SetCursorPos(ImVec2(280, 40));
                ImGui::Text("SALAM");
                ImGui::SetCursorPos(ImVec2(160, 60));
                ImGui::Text("amaliat morede nazar ra entekhab konid");

                ImGui::SetCursorPos(ImVec2(20, 100));
                if (ImGui::Button("sabt mahsool", ImVec2(250, 60)))
                {
                    dokme_sabt_mahsool = true;
                }

                ImGui::SetCursorPos(ImVec2(320, 100));
                if (ImGui::Button("Sabt sefaresh", ImVec2(250, 60)))
                {
                    dokme_Sabt_sefaresh = true;
                }

                ImGui::SetCursorPos(ImVec2(20, 165));
                if (ImGui::Button("Sefareshat", ImVec2(250, 60)))
                {
                    dokme_Sefareshat = true;
                }

                ImGui::SetCursorPos(ImVec2(320, 165));
                if (ImGui::Button("Gardesh mali", ImVec2(250, 60)))
                {
                    dokme_Gardesh_mali = true;
                }

                ImGui::SetCursorPos(ImVec2(150, 230));
                if (ImGui::Button("Mahsoolat", ImVec2(300, 60)))
                {
                    dokme_Mahsoolat = true;
                }

                ImGui::End();
            }
        }
        ImGui::SetNextWindowSize(ImVec2(200, 200));
        if (dokme_sabt_mahsool)
        {
            if (ImGui::Begin(
                    "sabt mahsool panjare", &dokme_sabt_mahsool,
                    ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize
                ))
            {
                ImGui::SliderInt("Gheimat", &gheimat, 0, 100000);
            }
            ImGui::End();
        }
        ImGui::SetNextWindowSize(ImVec2(300, 400));
        if (dokme_Sabt_sefaresh)
        {
            if (ImGui::Begin(
                    "Sabt sefaresh panjare", &dokme_Sabt_sefaresh,
                    ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize
                ))
            {
            }
            ImGui::End();
        }
        ImGui::SetNextWindowSize(ImVec2(400, 400));
        if (dokme_Sefareshat)
        {
            if (ImGui::Begin(
                    "Sefareshat panjare", &dokme_Sefareshat,
                    ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize
                ))
            {
            }
            ImGui::End();
        }
        ImGui::SetNextWindowSize(ImVec2(100, 500));
        if (dokme_Gardesh_mali)
        {
            if (ImGui::Begin(
                    "Gardesh mali panjare", &dokme_Gardesh_mali,
                    ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize
                ))
            {
            }
            ImGui::End();
        }
        ImGui::SetNextWindowSize(ImVec2(600, 800));
        if (dokme_Mahsoolat)
        {
            if (ImGui::Begin(
                    "Mahsoolat panjare", &dokme_Mahsoolat,
                    ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize
                ))
            {
            }
            ImGui::End();
        }
        ImGui::PopFont();
        // Rendering
        ImGui::EndFrame();
        g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
        D3DCOLOR clear_col_dx = D3DCOLOR_RGBA(
            (int)(clear_color.x * clear_color.w * 255.0f),
            (int)(clear_color.y * clear_color.w * 255.0f),
            (int)(clear_color.z * clear_color.w * 255.0f), (int)(clear_color.w * 255.0f)
        );
        g_pd3dDevice->Clear(
            0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0
        );
        if (g_pd3dDevice->BeginScene() >= 0)
        {
            ImGui::Render();
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            g_pd3dDevice->EndScene();
        }
        HRESULT result = g_pd3dDevice->Present(nullptr, nullptr, nullptr, nullptr);

        // Handle loss of D3D9 device
        if (result == D3DERR_DEVICELOST &&
            g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
            ResetDevice();
    }

    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);

    return 0;
}

// Helper functions

bool CreateDeviceD3D(HWND hWnd)
{
    if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == nullptr)
        return false;

    // Create the D3DDevice
    ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
    g_d3dpp.Windowed = TRUE;
    g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    g_d3dpp.BackBufferFormat =
        D3DFMT_UNKNOWN; // Need to use an explicit format with alpha if
                        // needing per-pixel alpha composition.
    g_d3dpp.EnableAutoDepthStencil = TRUE;
    g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE; // Present with vsync
    // g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;   //
    // Present without vsync, maximum unthrottled framerate
    if (g_pD3D->CreateDevice(
            D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
            D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice
        ) < 0)
        return false;

    return true;
}

void CleanupDeviceD3D()
{
    if (g_pd3dDevice)
    {
        g_pd3dDevice->Release();
        g_pd3dDevice = nullptr;
    }
    if (g_pD3D)
    {
        g_pD3D->Release();
        g_pD3D = nullptr;
    }
}

void ResetDevice()
{
    ImGui_ImplDX9_InvalidateDeviceObjects();
    HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
    if (hr == D3DERR_INVALIDCALL)
        IM_ASSERT(0);
    ImGui_ImplDX9_CreateDeviceObjects();
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(
    HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam
);

// Win32 message handler
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
        case WM_SIZE:
            if (wParam == SIZE_MINIMIZED)
                return 0;
            g_ResizeWidth = (UINT)LOWORD(lParam); // Queue resize
            g_ResizeHeight = (UINT)HIWORD(lParam);
            return 0;
        case WM_SYSCOMMAND:
            if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
                return 0;
            break;
        case WM_DESTROY:
            ::PostQuitMessage(0);
            return 0;
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}
// direct3d
