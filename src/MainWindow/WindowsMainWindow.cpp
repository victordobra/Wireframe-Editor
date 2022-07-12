#include "BuildInfo.hpp"

#ifdef PLATFORM_WINDOWS

/*
    ⚠️ LIVE FINGER REACTION ⚠️
⠀⠀⠀⠀⠀⠀⠀⣀⣴⣶⣿⣿⣿⣿⣿⣿⣿⣶⣦⣀⠀⠀⠀⠀⠀⠀   
      ⣤⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣄⠀⠀⠀   
⠀⠀⠀⢀⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣧⠀⠀⠀  
   ⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣟⣛⣻⣿⣿⣟⣿⣿⣿⣷⠀⠀  
   ⣿⣿⣿⣿⣿⣿⣿⣿⣿⣫⣽⣾⣻⣾⣿⣿⣿⣿⡿⣿⣿⠀⠀  
  ⢰⣿⣿⣻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠻⡿⠿⠟⠛⣟⣿⣽⠀⠀  
 ⠀⠸⣿⣿⣿⣷⣿⣿⣿⣿⡿⠍⠈⠀⠁⣴⡆⠀⠀⠠⢭⣮⣿⡶⠀
⡴⠲⣦⢽⣿⣿⣿⣿⣿⣟⣩⣨⣀⡄⣐⣾⣿⣿⣇⠠⣷⣶⣿⣿⡠⠁
⠃⢀⡄⠀⢻⣿⣿⣿⣿⣽⢿⣿⣯⣾⣿⣿⣿⣿⣿⢿⣿⣿⡟⣿⠀⠀
 ⠣⠧⠀⢿⣿⣿⣿⣿⣿⣿⣿⣿⠟⢸⣿⠿⠿⠿⣧⠙⣿⣿⡿⠀⠀
   ⠁⠼⣒⡿⣿⣿⣿⣿⣿⣿⣿⣠⣬⠀⠀⠀⠀⣾⣷⡈⣿⡇⠀
     ⠉⢳⣿⣿⣿⣿⣿⣿⣿⢟⠗⠼⠖⠒⠔⠉⠉⠻⣿⠇⠀
       ⠈⣻⡿⣿⣿⣿⣿⡿⡀⣤⡄⠸⣰⣾⡒⣷⣴⣿⠀
       ⠂⢸⡗⡄⠘⠭⣭⣷⣿⣮⣠⣌⣫⣿⣷⣿⣿⠃⠀
       ⠈⠀⢸⣿⣾⣷⣦⡿⣿⣿⣿⡿⢻⠞⣹⣿⣿⠏⠀
       ⢘⠀⠘⢻⡿⢿⣋⣤⣤⠌⠉⠛⠛⠀⠈⠉⠁⠀⠀
*/

#include "MainWindow/WindowsMainWindow.hpp"
#include "Vulkan/Device.hpp"
#include "Vulkan/SwapChain.hpp"
#include "IO/Cursor.hpp"
#include "Core.hpp"
#include "ProjectInfo.hpp"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tchar.h>

// Variables
wfe::string className = "Application";
wfe::string appName = PROJECT_NAME;

HINSTANCE hInstance;
HWND hWnd;

// WndProc predeclaration
LRESULT CALLBACK WinProc(_In_ HWND hWnd, _In_ UINT message, _In_ WPARAM wParam, _In_ LPARAM lParam);

// Internal helper functions
static void RegisterApplicationClass() {
    // Create the class info
    WNDCLASSEX wcex;

    wcex.cbSize        = sizeof(WNDCLASSEX);
    wcex.style         = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc   = WinProc;
    wcex.cbClsExtra    = 0;
    wcex.cbWndExtra    = 0;
    wcex.hInstance     = hInstance;
    wcex.hIcon         = LoadIcon(wcex.hInstance, IDI_APPLICATION);
    wcex.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = NULL;
    wcex.lpszMenuName  = NULL;
    wcex.lpszClassName = className.c_str();
    wcex.hIconSm       = LoadIcon(wcex.hInstance, IDI_APPLICATION);

    // Throw an error if the class wasn't registered
    if (!RegisterClassEx(&wcex))
        wfe::console::OutFatalError("Failed to register class!", 1);
}
static void CreateHWnd(wfe::int32_t nCmdShow) {
    // Create the window
    hWnd = CreateWindowEx(
        0,
        className.c_str(),
        appName.c_str(),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 
        CW_USEDEFAULT, CW_USEDEFAULT,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    // Throw an error if the window wasn't created
    if (!hWnd)
        wfe::console::OutFatalError("Failed to create window!", 1);

    // Show and update the window
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
}

// Windows functions
int WINAPI WinMain(_In_ HINSTANCE hInst, _In_opt_ HINSTANCE hPrevInst, _In_ LPSTR lpCmdLine, _In_ int nCmdShow) {
    hInstance = hInst;

    wfe::console::OpenLogFile();
    RegisterApplicationClass();
    CreateHWnd(nCmdShow);

    // Main message loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}
LRESULT CALLBACK WinProc(_In_ HWND hWindow, _In_ UINT message, _In_ WPARAM wParam, _In_ LPARAM lParam) {
    switch(message) {
    case WM_CREATE: 
        hWnd = hWindow;

        wfe::editor::CreateVulkanDevice();
        wfe::editor::CreateSwapChain({ wfe::editor::GetMainWindowWidth(), wfe::editor::GetMainWindowHeight() });

        return 0;
    case WM_PAINT: 
        wfe::editor::UpdateCursorType();
        return 0;
    case WM_CLOSE:
        wfe::editor::DeleteSwapChain();
        wfe::editor::DeleteVulkanDevice();

        wfe::console::CloseLogFile();

        DestroyWindow(hWindow);
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProc(hWindow, message, wParam, lParam);
    }

    return 0;
}

// External functions
wfe::size_t wfe::editor::GetMainWindowWidth() {
    RECT windowRect;
    WINBOOL result = GetWindowRect(hWnd, &windowRect);
    if(!result) {
        char_t error[256];
        FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), error, 256 * sizeof(char_t), NULL);
        console::OutFatalError((string)"Failed to obtain window rectangle! Error: " + error, 1);
    }

    return (size_t)(windowRect.right - windowRect.left);
}
wfe::size_t wfe::editor::GetMainWindowHeight() {
    RECT windowRect;
    WINBOOL result = GetWindowRect(hWnd, &windowRect);
    if(!result) {
        char_t error[256];
        FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), error, 256 * sizeof(char_t), NULL);
        console::OutFatalError((string)"Failed to obtain window rectangle! Error: " + error, 1);
    }

    return (size_t)(windowRect.bottom - windowRect.top);
}
wfe::ptrdiff_t wfe::editor::GetMainWindowXPos() {
    RECT windowRect;
    WINBOOL result = GetWindowRect(hWnd, &windowRect);
    if(!result) {
        char_t error[256];
        FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), error, 256 * sizeof(char_t), NULL);
        console::OutFatalError((string)"Failed to obtain window rectangle! Error: " + error, 1);
    }

    return (ptrdiff_t)windowRect.left;
}
wfe::ptrdiff_t wfe::editor::GetMainWindowYPos() {
    RECT windowRect;
    WINBOOL result = GetWindowRect(hWnd, &windowRect);
    if(!result) {
        char_t error[256];
        FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), error, 256 * sizeof(char_t), NULL);
        console::OutFatalError((string)"Failed to obtain window rectangle! Error: " + error, 1);
    }

    return (ptrdiff_t)windowRect.top;
}

wfe::string wfe::editor::GetMainWindowName() {
    return appName;
}
void wfe::editor::SetMainWindowName(const wfe::string& newName) {
    appName = newName;
    SetWindowText(hWnd, appName.c_str());
}

HWND wfe::editor::GetMainWindowHandle() {
    return hWnd;
}
HINSTANCE wfe::editor::GetWindowsInstance() {
    return hInstance;
}

#endif