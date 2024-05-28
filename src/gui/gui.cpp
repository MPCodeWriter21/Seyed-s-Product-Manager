#include "gui/gui.hpp"
#include "database/database.hpp"
#include "database/order.hpp"
#include "database/product.hpp"
#include "database/user.hpp"
#include "gui/imgui/imgui.h"
#include "gui/imgui/imgui_impl_dx9.h"
#include "gui/imgui/imgui_impl_win32.h"
#include "utils/argparse.hpp"
#include "utils/utils.hpp"
#include <d3d9.h>
#include <filesystem>
#include <iostream>
#include <string>
#include <tchar.h>

// Close Buttons
bool add_product_button = false;
bool new_order_button = false;
bool orders_button = false;
bool dokme_Gardesh_mali = false;
bool products_button = false;
bool theme_button = false;
bool dark_theme_button = true;

// Functions
void new_order(Orders &orders, Products &products);
void turnover(Orders &orders, Products &products, char *from_date, char *to_date);
void AddProduct(
    char *name_,
    char *price_,
    char *available_count_,
    char *descript_,
    Products &products
);
void ShowProducts(Products &);
void show_info_gui_order(Order order);
void show_info_gui_product(Product product);
void stylish ();
int run_gui(std::filesystem::path executable_path, Users &database)
{
    Users users((Users &)database);
    Products products((Products &)database);
    Orders orders((Orders &)database);

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
    io.Fonts->AddFontDefault();
    std::string font_path =
        (executable_path.parent_path() / "misc" / "Lalezar.Regular.ttf").string();
    ImFont *mainfont = io.Fonts->AddFontFromFileTTF(font_path.c_str(), 30);
    IM_ASSERT(mainfont != NULL);
    // Our state
    // ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    ImVec4 clear_color = ImVec4(40, 40, 40, 50);
    // ImVec4 clear_color = ImVec4(255, 255, 255, 200);

    // Main loop

    // Buttons
    bool done = false;

    // Login Variables
    bool logged = false;
    static char passput[128];
    bool wrong_pass = false;
    bool login_pass = false;

    static char available_count_[128];
    static char name_[128];
    static char price_[128];
    static char descript_[128];
    static char start_date_[128];
    static char end_date_[128];
    
// ImGuiStyle new_dark;
    stylish ();

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

        ImGui::PushFont(mainfont);
        //ImGui::SetNextWindowPos(ImVec2(600, 250));
        ImGui::SetNextWindowPos(ImVec2(300, 50));
        //ImGui::SetNextWindowSize(ImVec2(200, 200));
        ImGui::SetNextWindowSize(ImVec2(400, 300));
        if (login_pass == false)
        {
            if (ImGui::Begin(
                    "LOGIN", NULL,
                    ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                        ImGuiWindowFlags_NoMove
                ))
            {

                ImGui::Text("Enter Your Password");
                bool log = ImGui::InputText(
                    "##", passput, sizeof(passput),
                    ImGuiInputTextFlags_Password | ImGuiInputTextFlags_EnterReturnsTrue
                );
                if (wrong_pass == true)
                {
                    ImGui::Text("Wrong Password");
                }
                if (ImGui::Button("login") or log)
                {
                    if (users.get_user(1)->check_password(Password::from_string(passput)
                        ))
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
        // Main Page
        if (logged)
        {
            login_pass = true;
            if (ImGui::Begin(
                    "Main Window", NULL,
                    ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                        ImGuiWindowFlags_NoMove
                ))
            {
                // ImGui::SetWindowFontScale(2.0f);
                ImGui::SetCursorPos(ImVec2(280, 40));
                ImGui::Text("Hello!");
                ImGui::SetCursorPos(ImVec2(200, 60));
                ImGui::Text("Please choose the action:");

                ImGui::SetCursorPos(ImVec2(20, 100));
                if (ImGui::Button("New Product", ImVec2(250, 60)))
                {
                    add_product_button = true;
                }

                ImGui::SetCursorPos(ImVec2(320, 100));
                if (ImGui::Button("New Order", ImVec2(250, 60)))
                {
                    new_order_button = true;
                }

                ImGui::SetCursorPos(ImVec2(20, 165));
                if (ImGui::Button("List of Orders", ImVec2(250, 60)))
                {
                    orders_button = true;
                }

                ImGui::SetCursorPos(ImVec2(320, 165));
                if (ImGui::Button("Gardesh mali", ImVec2(250, 60)))
                {
                    dokme_Gardesh_mali = true;
                }

                ImGui::SetCursorPos(ImVec2(150, 230));
                if (ImGui::Button("Products", ImVec2(300, 60)))
                {
                    products_button = true;
                }
                ImGui::SetCursorPos(ImVec2(20, 40));
                if (ImGui::Button("Theme", ImVec2(70, 40)))
                {
                    theme_button = true;
                }

                ImGui::End();
            }
        }
        // New Product Window
        ImGui::SetNextWindowSize(ImVec2(350, 250));
        if (add_product_button)
        {
            if (ImGui::Begin(
                    "New Product Window", &add_product_button,
                    ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize
                ))
            {
                // ImGui::SliderInt("Price", &price, 0, 100000);
                ImGui::InputText(
                    "Name", name_, sizeof(passput), ImGuiInputTextFlags_EnterReturnsTrue
                );
                ImGui::InputText(
                    "Price", price_, sizeof(passput),
                    ImGuiInputTextFlags_EnterReturnsTrue
                );
                ImGui::InputText(
                    "Count(s)", available_count_, sizeof(passput),
                    ImGuiInputTextFlags_EnterReturnsTrue
                );
                ImGui::InputText(
                    "Descript", descript_, sizeof(passput),
                    ImGuiInputTextFlags_EnterReturnsTrue
                );
                if (ImGui::Button("Save"))
                {
                    AddProduct(name_, price_, available_count_, descript_, products);
                    add_product_button = false;
                }
            }
            ImGui::End();
        }
        // New Order Window
        ImGui::SetNextWindowSize(ImVec2(300, 400));
        if (new_order_button)
        {
            if (ImGui::Begin(
                    "New Order Window", &new_order_button,
                    ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize
                ))
            {
                new_order(orders, products); //, new_order_button);
            }
            ImGui::End();
        }
        // Orders Window
        ImGui::SetNextWindowSize(ImVec2(400, 400));
        if (orders_button)
        {
            if (ImGui::Begin(
                    "Orders Window", &orders_button,
                    ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize
                ))
            {
                const std::vector<Order> &list_of_orders = orders.list_orders(products);
                for (unsigned int i = 0; i < list_of_orders.size(); i++)
                {
                    show_info_gui_order(list_of_orders[i]);
                    std::cout << std::endl;
                }
            }
            ImGui::End();
        }
        // Gardesh mali panjare
        ImGui::SetNextWindowSize(ImVec2(350, 500));
        if (dokme_Gardesh_mali)
        {
            if (ImGui::Begin(
                    "Gardesh mali panjare", &dokme_Gardesh_mali,
                    ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize
                ))
            {
                ImGui::InputText(
                    ": From", start_date_, sizeof(passput),
                    ImGuiInputTextFlags_EnterReturnsTrue
                );
                ImGui::InputText(
                    ": To", end_date_, sizeof(passput),
                    ImGuiInputTextFlags_EnterReturnsTrue
                );
                turnover(orders, products, start_date_, end_date_);
            }
            ImGui::End();
        }
        // Products Window
        ImGui::SetNextWindowSize(ImVec2(600, 600));
        if (products_button)
        {
            if (ImGui::Begin(
                    "Products Window", &products_button,
                    ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize
                ))
            {
                const std::vector<Product> &list_of_products = products.list_products();
                for (unsigned int i = 0; i < list_of_products.size(); i++)
                {
                    show_info_gui_product(list_of_products[i]);
                    ImGui::Separator();
                }
            }
            ImGui::End();
        }
        // Themes
        ImGui::SetNextWindowSize(ImVec2(165, 120));
        if (theme_button)
        {
            if (ImGui::Begin(
                    "Themes", &theme_button,
                    ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize
                ))
            {
                if (dark_theme_button)
                {
                    if (ImGui::Button("Light", ImVec2(130, 50)))
                    {
                        ImGui::StyleColorsLight();
                        dark_theme_button = false;
                    }
                }
                else
                {
                    if (ImGui::Button("Dark", ImVec2(130, 50)))
                    {
                        stylish ();
                        // ImGui::StyleColorsDark();
                        // //ImGui::PushStyle(style);
                        dark_theme_button = true;
                    }
                }
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

void AddProduct(
    char *name_,
    char *price_,
    char *available_count_,
    char *descript_,
    Products &products
)
{
    if (name_ == NULL)
    {
        ImGui::Text("You need to specify a product-name to add a product.");
    }
    if (price_ == NULL)
    {
        ImGui::Text("You need to specify a price for the product.");
    }
    if (available_count_ == nullptr)
    {
        ImGui::Text("How many products do we have available? Please tell me.");
    }
    // if (description_ == nullptr)
    // {
    //     description_ = new std::string("");
    // }
    products.add_product(
        name_, std::stod(price_), std::stoi(available_count_), descript_
    );
    // delete description;
}

void show_info_gui_product(Product product)
{
    ImGui::Text(
        "%s", ("Product ID               : " + std::to_string(product.get_id())).c_str()
    );
    ImGui::Text("%s", ("Name                     : " + product.get_name()).c_str());
    ImGui::Text(
        "%s",
        ("Price                    : " + std::to_string(product.get_price())).c_str()
    );
    ImGui::Text(
        "%s",
        ("No. of Available Products: " + std::to_string(product.get_available_count()))
            .c_str()
    );
    if (product.get_description() != "")
        ImGui::Text(
            "%s", ("Product Descriptions     : " + product.get_description()).c_str()
        );
}

void show_info_gui_order(Order order)
{
    ImGui::Text(
        "%s", ("Order ID               : " + std::to_string(order.get_id())).c_str()
    );
    ImGui::Text(
        "%s", ("Total price            : " + std::to_string(order.get_total())).c_str()
    );
    if (order.get_total() != order.get_total_after_discount())
        ImGui::Text(
            "%s", ("Total with " + std::to_string(order.get_discount()) +
                    "% discount: " + std::to_string(order.get_total_after_discount()))
                    .c_str()
        );
    if (order.is_paid())
        ImGui::Text(
            "%s", ("Paid in `" + order.get_pay_date() + "` at `" +
                    order.get_pay_time() + "`.")
                    .c_str()
        );
    else
        ImGui::Text("Is Paid                : No");
    ImGui::Text(
        "%s", ("No. of Orders          : " + std::to_string(order.get_count())).c_str()
    );
}

void turnover(Orders &orders, Products &products, char *from_date, char *to_date)
{
    std::vector<Order> list_of_orders;
    if (from_date == nullptr && to_date == nullptr)
        list_of_orders =
            orders.get_orders({"now", "-1 month", "localtime"}, {}, products);
    else if (from_date != nullptr && to_date == nullptr)
        list_of_orders = orders.get_orders({from_date, "localtime"}, {}, products);
    else if (from_date == nullptr && to_date != nullptr)
        list_of_orders = orders.get_orders({}, {to_date, "localtime"}, products);
    else
        list_of_orders = orders.get_orders(
            {from_date, "localtime"}, {to_date, "localtime"}, products
        );
    if (list_of_orders.size() < 1)
    {
        ImGui::Text("We did not get any payments\n in that time frame :(");
        new_order_button = false;
    }
    ImGui::Text(
        "%s", ("We got " + std::to_string(list_of_orders.size()) + " payments!").c_str()
    );
    double total_profit = 0;
    double total_no_discount_sales = 0;
    for (size_t i = 0; i < list_of_orders.size(); i++)
    {
        total_no_discount_sales += list_of_orders[i].get_total();
        total_profit += list_of_orders[i].get_total_after_discount();
    }
    ImGui::Text("%s", ("We earned " + std::to_string(total_profit)).c_str());
    if (total_profit != total_no_discount_sales)
        ImGui::Text(
            "%s",
            (" but we could have earned " + std::to_string(total_no_discount_sales) +
            " if you hadn't given the customers\n so much discounts")
                .c_str()
        );
    new_order_button = false;
}

void new_order(Orders &orders, Products &products)
{
    std::vector<ProductOrder> product_orders;
    int product_id, count;
    while (true)
    {
        ImGui::Text("Enter the product id : ");
        ImGui::InputInt(
            "##", &product_id, sizeof(product_id), ImGuiInputTextFlags_EnterReturnsTrue
        );
        if (product_id == 0)
            break;
        Product *product = products.get_product(product_id);
        if (product == nullptr)
        {
            ImGui::Text(
                "%s",
                ("Product with id " + std::to_string(product_id) + " does not exist!")
                    .c_str()
            );
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
            ImGui::Text("Enter the number of products: ");
        else
            ImGui::Text(
                "%s",
                ("Enter the number of products(Currently " +
                std::to_string(product_orders[product_order_index].count) + "): ")
                    .c_str()
            );
        ImGui::InputInt(
            "##", &count, sizeof(count), ImGuiInputTextFlags_EnterReturnsTrue
        );
        if (count < 1)
        {
            ImGui::Text("No product was added to the order!");
            continue;
        }
        if ((unsigned int)count > product->get_available_count())
        {
            ImGui::Text(
                "%s", ("Sorry but there are only " +
                        std::to_string(product->get_available_count()) +
                        " of this product left in stock!")
                        .c_str()
            );
            continue;
        }
        if (product_order_index == -1)
            product_orders.push_back(ProductOrder(*product, count));
        else
            product_orders[product_order_index].count = count;
    }
    if (product_orders.size() < 1)
    {
        ImGui::Text("No product means no order!");
        new_order_button = false;
    }
    int discount = 0;
    ImGui::Text("Any discounts? If not just enter 0: ");
    while (true)
    {
        ImGui::InputInt(
            "##", &discount, sizeof(discount), ImGuiInputTextFlags_EnterReturnsTrue
        );
        if (discount < 0 || discount > 100)
        {
            ImGui::Text(
                "%s",
                ("Sorry but it's not possible to have a " + std::to_string(discount) +
                "% discount!\n" + "Maybe try again? Discount(0-100): ")
                    .c_str()
            );
        }
        else
            break;
    }
    // std::string phone_number, check;
    static char phone_number[128], check[128];
    ImGui::Text("Please enter the customer's phone number(e.g. +989112223333): ");
    while (true)
    {
        ImGui::InputText(
            "##", phone_number, sizeof(phone_number), ImGuiInputTextFlags_EnterReturnsTrue
        );
        if (validate_phone_number(phone_number))
            break;
        else
        {
            ImGui::Text("Sorry but I don't recognize this phone number format...\nAre "
                        "you sure that's correct?(Y/n): ");
            ImGui::InputText(
                "##", check, sizeof(check), ImGuiInputTextFlags_EnterReturnsTrue
            );
            if (std::string(check) == "y" || std::string(check) == "Y")
                break;
            ImGui::Text("Please enter a valid phone number(e.g. +989445556666): ");
        }
    }
    ImGui::Text("Creating a new order...");
    orders.add_order(product_orders, phone_number, discount);
}
void stylish ()
{
    ImGui::StyleColorsDark();
    ImGuiStyle &style = ImGui::GetStyle();
    style.WindowPadding = ImVec2(15, 15);
	style.WindowRounding = 5.0f;
	style.FramePadding = ImVec2(5, 5);
	style.FrameRounding = 4.0f;
	style.ItemSpacing = ImVec2(12, 8);
	style.ItemInnerSpacing = ImVec2(8, 6);
	style.IndentSpacing = 25.0f;
	style.ScrollbarSize = 15.0f;
	style.ScrollbarRounding = 9.0f;
	style.GrabMinSize = 5.0f;
	style.GrabRounding = 3.0f;
	style.Colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.80f, 0.80f, 0.83f, 0.88f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.40f, 0.39f, 0.38f, 0.16f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.40f, 0.39f, 0.38f, 0.39f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.40f, 0.39f, 0.38f, 1.00f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
	style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);
}