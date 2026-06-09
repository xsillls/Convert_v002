#pragma once
#include <Windows.h>
#include "../ImGui/imgui.h"
#include "../ImGui/backends/imgui_impl_win32.h"
#include "../ImGui/backends/imgui_impl_dx11.h"

#include <d3d11.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dwmapi.lib")
#include <string>

#include "../System/System.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(
    HWND, UINT, WPARAM, LPARAM);

extern HWND g_hwnd;



// ================= DX11


extern ID3D11Device* g_Device;
extern ID3D11DeviceContext* g_Context;
extern IDXGISwapChain* g_SwapChain;
extern ID3D11RenderTargetView* g_RTV;


// ================= BLUR (DWM)
enum ACCENT_STATE
{
    ACCENT_DISABLED = 0,
    ACCENT_ENABLE_BLURBEHIND = 3
};

struct ACCENT_POLICY
{
    int AccentState;
    int AccentFlags;
    int GradientColor;
    int AnimationId;
};

enum WINDOWCOMPOSITIONATTRIB
{
    WCA_ACCENT_POLICY = 19
};

struct WINDOWCOMPOSITIONATTRIBDATA
{
    WINDOWCOMPOSITIONATTRIB Attrib;
    PVOID pvData;
    SIZE_T cbData;
};

inline void CreateRenderTarget()
{
    ID3D11Texture2D* bb = nullptr;
    g_SwapChain->GetBuffer(0, IID_PPV_ARGS(&bb));
    g_Device->CreateRenderTargetView(bb, nullptr, &g_RTV);
    bb->Release();
}

void Menu();

std::string Logo_Anim(std::string, int seting=0);
void Button_close(ImDrawList*);
void Uploid_file();

void DrawFileIcon(float size);
bool DrawStyledButton(const char* label, ImVec2 size);
bool DrawStyledSlider(const char* id, float* value, float min, float max, ImVec2 size);
void DrawProgressBar(const char* id, float progress, ImVec2 size);