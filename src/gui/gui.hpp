#include <d3d9.h>
#include <tchar.h>

// Data
static LPDIRECT3D9 g_pD3D = nullptr;
static LPDIRECT3DDEVICE9 g_pd3dDevice = nullptr;
static UINT g_ResizeWidth = 0, g_ResizeHeight = 0;
static D3DPRESENT_PARAMETERS g_d3dpp = {};

// Font addresses
extern char font_file_start[] asm("_binary_misc_Lalezar_Regular_ttf_start");
extern char font_file_end[] asm("_binary_misc_Lalezar_Regular_ttf_end");

bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void ResetDevice();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
int run_gui();
