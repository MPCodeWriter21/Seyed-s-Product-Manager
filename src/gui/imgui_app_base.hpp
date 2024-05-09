#include "gui/imgui/imgui.h"
#include <d3d9.h>

// Data
static LPDIRECT3D9 g_pD3D = nullptr;
static LPDIRECT3DDEVICE9 g_pd3dDevice = nullptr;
static UINT g_ResizeWidth = 0, g_ResizeHeight = 0;
static D3DPRESENT_PARAMETERS g_d3dpp = {};

bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void ResetDevice();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

class ImGUIApp
{
  public:
    ImGUIApp();

    ~ImGUIApp();

    void Run();

    virtual void StartUp() = 0;
    virtual void Update() = 0;

    bool initiated_successfully = false;

  protected:
    WNDCLASSEXW wc;
    HWND hwnd;
    ImVec4 *clear_color;
    ImGuiIO *io;
    bool done = false;
};
