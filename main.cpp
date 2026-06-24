#include "Menu/Menu.h"
#include <dwmapi.h>
#include <ole2.h>

#pragma comment(lib,"dwmapi.lib")
#pragma comment(lib,"ole32.lib")

HWND g_hwnd = nullptr;
ID3D11Device* g_Device = nullptr;
ID3D11DeviceContext* g_Context = nullptr;
IDXGISwapChain* g_SwapChain = nullptr;
ID3D11RenderTargetView* g_RTV = nullptr;

static bool g_minimized = false;

bool g_IsDraggingFile = false;
bool g_FileDropped = false;
std::string g_FilePath;

int Button_Menu = 0;
bool Button_Menu_Extract = false;


void EnableBlur(HWND hwnd)
{
    ACCENT_POLICY policy{};
    policy.AccentState = 4; 
    policy.GradientColor = 0x990000;

    WINDOWCOMPOSITIONATTRIBDATA data{};
    data.Attrib = WCA_ACCENT_POLICY;
    data.pvData = &policy;
    data.cbData = sizeof(policy);

    auto fn = (BOOL(WINAPI*)(HWND, WINDOWCOMPOSITIONATTRIBDATA*))
        GetProcAddress(GetModuleHandleW(L"user32.dll"),
            "SetWindowCompositionAttribute");

    if (fn)
        fn(hwnd, &data);
}



// ================= Cleanup

void CleanupRenderTarget()
{
    if (g_RTV)
    {
        g_RTV->Release();
        g_RTV = nullptr;
    }
}

void CleanupDeviceD3D()
{
    CleanupRenderTarget();

    if (g_SwapChain)
    {
        g_SwapChain->Release();
        g_SwapChain = nullptr;
    }

    if (g_Context)
    {
        g_Context->Release();
        g_Context = nullptr;
    }

    if (g_Device)
    {
        g_Device->Release();
        g_Device = nullptr;
    }
}



// ================= DX11

bool CreateDeviceD3D(HWND hwnd)
{
    DXGI_SWAP_CHAIN_DESC sd{};
    sd.BufferCount = 2;

    sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;

    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hwnd;

    sd.SampleDesc.Count = 1;

    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    D3D_FEATURE_LEVEL level;
    const D3D_FEATURE_LEVEL levels[] = { D3D_FEATURE_LEVEL_11_0 };

    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        D3D11_CREATE_DEVICE_BGRA_SUPPORT,
        levels,
        1,
        D3D11_SDK_VERSION,
        &sd,
        &g_SwapChain,
        &g_Device,
        &level,
        &g_Context
    );

    if (FAILED(hr))
        return false;

    CreateRenderTarget();
    return true;
}



// ================= WindowProc

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {

    case WM_NCHITTEST:
        return HTCLIENT;

    case WM_SIZE:

        if (wParam == SIZE_MINIMIZED)
        {
            g_minimized = true;
        }
        else
        {
            g_minimized = false;

            if (g_Device)
            {
                CleanupRenderTarget();

                g_SwapChain->ResizeBuffers(
                    0,
                    (UINT)LOWORD(lParam),
                    (UINT)HIWORD(lParam),
                    DXGI_FORMAT_UNKNOWN,
                    0
                );

                CreateRenderTarget();
            }
        }

        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}



// ================= WinMain

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int)
{
    const wchar_t* CLASS = L"RealGlass";

    WNDCLASS wc{};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInst;
    wc.lpszClassName = CLASS;
    wc.hbrBackground = NULL;

    RegisterClass(&wc);



    HWND hwnd = CreateWindowEx(
        WS_EX_APPWINDOW,
        CLASS,
        L"",
        WS_POPUP,
        (GetSystemMetrics(SM_CXSCREEN) - 600) / 2,
        (GetSystemMetrics(SM_CYSCREEN) - 350) / 2,
        600,
        350,
        nullptr,
        nullptr,
        hInst,
        nullptr
    );

    g_hwnd = hwnd;



    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);



    // ================= Drag & Drop

    OleInitialize(nullptr);

    IDropTarget* dropTarget = CreateDropTarget();
    RegisterDragDrop(hwnd, dropTarget);



    // ================= Windows blur

    EnableBlur(hwnd);



    // ================= Rounded corners

    DWM_WINDOW_CORNER_PREFERENCE corner = DWMWCP_ROUND;

    DwmSetWindowAttribute(
        hwnd,
        DWMWA_WINDOW_CORNER_PREFERENCE,
        &corner,
        sizeof(corner)
    );



    if (!CreateDeviceD3D(hwnd))
        return 0;



    // ================= ImGui

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(g_Device, g_Context);



    bool done = false;
    MSG msg{};

    while (!done)
    {

        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                done = true;

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (done)
            break;


        if (g_minimized)
        {
            Sleep(50);
            continue;
        }



        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();



        ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
        Menu();



        ImGui::Render();



        const float clear_color[4] = { 0,0,0,0 };

        g_Context->OMSetRenderTargets(1, &g_RTV, nullptr);
        g_Context->ClearRenderTargetView(g_RTV, clear_color);

        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());



        g_SwapChain->Present(1, 0);
    }



    // ================= Shutdown

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();



    RevokeDragDrop(hwnd);
    OleUninitialize();



    CleanupDeviceD3D();
    DestroyWindow(hwnd);



    return 0;
}