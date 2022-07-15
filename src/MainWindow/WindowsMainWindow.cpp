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
#include "GUI/GUIPipeline.hpp"
#include "Config/EditorColors.hpp"
#include "Core.hpp"
#include "ProjectInfo.hpp"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tchar.h>

// Constants
const wfe::size_t MIN_WINDOW_WIDTH = 200, MIN_WINDOW_HEIGHT = 200;

// Variables
wfe::string className = "Application";
wfe::string appName = PROJECT_NAME;

wfe::size_t windowWidth, windowHeight;
wfe::ptrdiff_t windowXPos, windowYPos;
wfe::bool8_t windowMaximized, windowMinimized;

HINSTANCE hInstance;
HWND hWnd;

// WndProc predeclaration
LRESULT CALLBACK WinProc(_In_ HWND hWnd, _In_ UINT message, _In_ WPARAM wParam, _In_ LPARAM lParam);

// Internal helper functions
static COLORREF HexColorToWin32Hex(uint32_t color) {
    uint8_t red = (color >> 16) & 0xff;
    uint8_t green = (color >> 8) & 0xff;
    uint8_t blue = color & 0xff;

    return RGB(red, green, blue);
}

static void RegisterApplicationClass() {
    // Load the editor colors
    wfe::editor::LoadColors();

    // Create the class info
    WNDCLASSEX wcex;

    wcex.cbSize        = sizeof(WNDCLASSEX);
    wcex.style         = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc   = WinProc;
    wcex.cbClsExtra    = 0;
    wcex.cbWndExtra    = 0;
    wcex.hInstance     = hInstance;
    wcex.hIcon         = LoadIcon(hInstance, IDI_APPLICATION);
    wcex.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = NULL;
    wcex.lpszMenuName  = NULL;
    wcex.lpszClassName = className.c_str();
    wcex.hIconSm       = LoadIcon(hInstance, IDI_APPLICATION);

    // Try to register the class
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
static void UpdateWindowInfo() {
    RECT windowRect;
    WINBOOL result = GetWindowRect(hWnd, &windowRect);
    if(!result) {
        wfe::char_t error[256];
        FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), error, 256 * sizeof(wfe::char_t), NULL);
        wfe::console::OutFatalError((wfe::string)"Failed to obtain window rectangle! Error: " + error, 1);
    }

    windowWidth = (size_t)(windowRect.right - windowRect.left);
    windowHeight = (size_t)(windowRect.bottom - windowRect.top);
    windowXPos = (ptrdiff_t)windowRect.left;
    windowYPos = (ptrdiff_t)windowRect.top;
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
    
        UpdateWindowInfo();
        wfe::editor::CreateVulkanDevice();
        wfe::editor::CreateSwapChain({ (uint32_t)windowWidth, (uint32_t)windowHeight });
        wfe::editor::CreateGUIPipeline();

        return 0;
    case WM_GETMINMAXINFO:
    {
        MINMAXINFO* minMaxInfo = (MINMAXINFO*)lParam;
        minMaxInfo->ptMinTrackSize = { MIN_WINDOW_WIDTH, MIN_WINDOW_HEIGHT };
    }

        return 0;
    case WM_SIZE:
        switch (wParam) {
        case SIZE_MINIMIZED:
            windowMaximized = false;
            windowMinimized = true;

            break;
        case SIZE_MAXIMIZED:
            windowXPos = 0;
            windowYPos = 0;
            windowWidth = (size_t)LOWORD(lParam);
            windowHeight = (size_t)HIWORD(lParam);
            windowMaximized = true;
            windowMinimized = false;

            wfe::editor::RecreateSwapChain({ windowWidth, windowHeight });

            break;
        case SIZE_RESTORED:
            if(windowMaximized || windowMinimized) {
                windowMaximized = false;
                windowMinimized = false;
                UpdateWindowInfo();

                wfe::editor::RecreateSwapChain({ windowWidth, windowHeight });
            }
            break;
        }

        return 0;
    case WM_EXITSIZEMOVE:
    {
        size_t oldWindowWidth = windowWidth;
        size_t oldWIndowHeight = windowHeight;

        UpdateWindowInfo();

        // Check the new window dimensions with the old ones and recreate the window if they are different
        if(windowWidth != oldWindowWidth || windowHeight != oldWIndowHeight)
            wfe::editor::RecreateSwapChain({ (uint32_t)windowWidth, (uint32_t)windowHeight });
    }

        return 0;
    case WM_PAINT:
        if(windowMinimized)
            return 0;

        wfe::editor::Draw();
        wfe::editor::UpdateCursorType();
        return 0;
    case WM_CLOSE:
        wfe::editor::DeleteGUIPipeline();
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
    return windowWidth;
}
wfe::size_t wfe::editor::GetMainWindowHeight() {
    return windowHeight;
}
wfe::ptrdiff_t wfe::editor::GetMainWindowXPos() {
    return windowXPos;
}
wfe::ptrdiff_t wfe::editor::GetMainWindowYPos() {
    return windowYPos;
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