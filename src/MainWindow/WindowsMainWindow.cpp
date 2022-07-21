#include "BuildInfo.hpp"

#ifdef PLATFORM_WINDOWS

#include "ProjectInfo.hpp"
#include "Core.hpp"

// WIndows include
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

// Variables
wfe::string className = "Application";
wfe::string appName = PROJECT_NAME;

wfe::size_t windowWidth, windowHeight;

HWND hWnd;
HINSTANCE hInstance;

// WinProc predeclaartion
LRESULT CALLBACK WinProc(_In_ HWND hWnd, _In_ UINT msg, _In_ WPARAM wParam, _In_ LPARAM lParam);

// Internal helper functions
static void OutputLastWin32Error(const wfe::char_t* output) {
    wfe::char_t error[256];
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), error, 256 * sizeof(wfe::char_t), NULL);
    wfe::console::OutFatalError((wfe::string)output + " Error: " + error, 1);
}

static void RegisterClass() {
    // Create the class register info
    WNDCLASSEX wcex;

    wcex.cbSize        = sizeof(WNDCLASSEX);
    wcex.style         = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc   = WinProc;
    wcex.cbClsExtra    = 0;
    wcex.cbWndExtra    = 0;
    wcex.hInstance     = hInstance;
    wcex.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = NULL;
    wcex.lpszMenuName  = NULL;
    wcex.lpszClassName = className.c_str();
    wcex.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

    // Try to register the class
    if(!RegisterClassEx(&wcex))
        OutputLastWin32Error("Failed to register class!");
}
static void CreateHWnd() {
    // Create the window
    hWnd = CreateWindow(
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

    // Throw an error if the window wasn't created properly
    if(!hWnd)
        OutputLastWin32Error("Failed to create window!");
    
    // Show and update the window
    ShowWindow(hWnd, SW_SHOWNORMAL);
    if(!UpdateWindow(hWnd))
        OutputLastWin32Error("Failed to update window!");
    
}
static wfe::int32_t RunMessageLoop() {
    MSG msg;
    while(GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (wfe::int32_t)msg.wParam;
}

// Wndows functions
int main(int argc, char** args) {
    hInstance = GetModuleHandle(NULL);

    wfe::console::OpenLogFile();
    RegisterClass();
    CreateHWnd();
    return RunMessageLoop();
}
LRESULT CALLBACK WinProc(_In_ HWND hWnd, _In_ UINT msg, _In_ WPARAM wParam, _In_ LPARAM lParam) {
    switch(msg) {
    case WM_CREATE:
        return 0;
    case WM_PAINT:
        return 0;
    case WM_CLOSE:
        DestroyWindow(hWnd);
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}

#endif