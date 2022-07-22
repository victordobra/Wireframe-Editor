#include "BuildInfo.hpp"

#ifdef PLATFORM_WINDOWS

#include "ProjectInfo.hpp"
#include "imgui.hpp"
#include "Core.hpp"
#include "Win32ToImGui.hpp"

// Windows include
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <windowsx.h>

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
        // Configure ImGui
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        ImGui::GetIO().BackendPlatformName = "WFE-Win32-Backend";
        ImGui::GetIO().BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
        ImGui::GetIO().BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

        ImGui::StyleColorsDark();
        
        break;
    case WM_SIZE:
        ImGui::GetIO().DisplaySize = { (wfe::float32_t)GET_X_LPARAM(lParam), (wfe::float32_t)GET_Y_LPARAM(lParam) };
        break;
    case WM_MOUSEMOVE:
        ImGui::GetIO().AddMousePosEvent((wfe::float32_t)GET_X_LPARAM(lParam), (wfe::float32_t)GET_Y_LPARAM(lParam));
        break;
    case WM_LBUTTONDOWN: 
    case WM_LBUTTONDBLCLK:
        ImGui::GetIO().AddMouseButtonEvent(ImGuiMouseButton_Left, true);
        break;
    case WM_RBUTTONDOWN: 
    case WM_RBUTTONDBLCLK:
        ImGui::GetIO().AddMouseButtonEvent(ImGuiMouseButton_Right, true);
        break;
    case WM_MBUTTONDOWN: 
    case WM_MBUTTONDBLCLK:
        ImGui::GetIO().AddMouseButtonEvent(ImGuiMouseButton_Middle, true);
        break;
    case WM_LBUTTONUP:
        ImGui::GetIO().AddMouseButtonEvent(ImGuiMouseButton_Left, false);
        break;
    case WM_RBUTTONUP:
        ImGui::GetIO().AddMouseButtonEvent(ImGuiMouseButton_Right, false);
        break;
    case WM_MBUTTONUP:
        ImGui::GetIO().AddMouseButtonEvent(ImGuiMouseButton_Middle, false);
        break;
    case WM_MOUSEWHEEL:
        ImGui::GetIO().AddMouseWheelEvent(0.f, (wfe::float32_t)GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA);
        break;
    case WM_MOUSEHWHEEL:
        ImGui::GetIO().AddMouseWheelEvent((wfe::float32_t)GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA, 0.f);
        break;
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
    {
        ImGuiKey key = Win32KeyToImGuiKey(wParam);
        wfe::int32_t scancode = (wfe::int32_t)LOBYTE(HIWORD(lParam));

        if(key != ImGuiKey_None) {
            ImGui::GetIO().AddKeyEvent(key, true);
            ImGui::GetIO().SetKeyEventNativeData(key, wParam, scancode);
        }
    }
        break;
    case WM_KEYUP:
    case WM_SYSKEYUP:
    {
        ImGuiKey key = Win32KeyToImGuiKey(wParam);
        wfe::int32_t scancode = (wfe::int32_t)LOBYTE(HIWORD(lParam));

        if(key != ImGuiKey_None) {
            ImGui::GetIO().AddKeyEvent(key, false);
            ImGui::GetIO().SetKeyEventNativeData(key, wParam, scancode);
        }
    }
        break;
    case WM_SETFOCUS:
        ImGui::GetIO().AddFocusEvent(true);
        break;
    case WM_KILLFOCUS:
        ImGui::GetIO().AddFocusEvent(false);
        break;
    case WM_CHAR:
        ImGui::GetIO().AddInputCharacter((wfe::char_t)wParam);
        break;
    case WM_SETCURSOR:
    {
        if(LOWORD(lParam) != HTCLIENT)
            break;

        if(ImGui::GetMouseCursor() == ImGuiMouseCursor_None || ImGui::GetIO().MouseDrawCursor) {
            SetCursor(NULL);
            return 1;
        }

        // Get the win32 cursor type
        LPTSTR cursorType = ImGuiCursorToWin32CursorType(ImGui::GetMouseCursor());

        // Load the cursor
        HCURSOR cursor = LoadCursor(NULL, cursorType);
        if(!cursor)
            OutputLastWin32Error("Failed to load cursor!");

        // Set the cursor
        if(!SetCursor(cursor))
            OutputLastWin32Error("Failed to set cursor!");
    }
        return 1;
    case WM_PAINT:
        break;
    case WM_CLOSE:
        DestroyWindow(hWnd);
        break;
    case WM_DESTROY:
        ImGui::DestroyContext();

        PostQuitMessage(0);
        break;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}

#endif