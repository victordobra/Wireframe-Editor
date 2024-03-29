#include "BuildInfo.hpp"

#ifdef PLATFORM_WINDOWS

#include "MainWindow/WindowsMainWindow.hpp"
#include "Vulkan/Device.hpp"
#include "Vulkan/SwapChain.hpp"
#include "Vulkan/ImGuiPipeline.hpp"
#include "Base/Window.hpp"
#include "Windows/EditorPropertiesWindow.hpp"
#include "ProjectInfo.hpp"
#include "imgui.hpp"
#include "Core.hpp"
#include "Win32ToImGui.hpp"

// Windows include
#include <windows.h>
#include <windowsx.h>
#include <shlobj.h>

// Variables
wfe::string className = "Application";
wfe::string appName;

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

// Internal functions
static const wfe::char_t* GetClipboardText(void* userData) {
    // Open the clipboard
    OpenClipboard(NULL);

    // Get the clipboard data
    HANDLE data = GetClipboardData(CF_TEXT);

    // Copy the data to a buffer
    wfe::char_t* dataPtr = (wfe::char_t*)GlobalLock(data);
    wfe::size_t size = strlen(dataPtr) + 1;

    wfe::char_t* buffer = new wfe::char_t[size];
    memcpy(buffer, dataPtr, size);

    // Close the clipboard
    GlobalUnlock(data);
    CloseClipboard();

    return buffer;
}
static void SetClipboardText(void* userData, const wfe::char_t* text) {
    // Open the clipboard
    OpenClipboard(NULL);

    // Copy the text to a buffer
    wfe::size_t size = strlen(text) + 1;
    HGLOBAL data = GlobalAlloc(GMEM_MOVEABLE, size);

    memcpy(GlobalLock(data), text, size);
    GlobalUnlock(data);

    // Set the clipboard text
    SetClipboardData(CF_TEXT, data);

    // Close the clipboard
    CloseClipboard();
}

static void RegisterWindowClass() {
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
        OutputLastWin32Error("Failed to register Win32 class!");
    wfe::console::OutMessageFunction("Registered Win32 class successfully.");
}
static void CreateHWnd() {
    wfe::editor::LoadWorkspace();
    wfe::editor::LoadEditorProperties();
    
    appName = wfe::editor::GetWorkspaceDir() + " - Wireframe Engine";
    
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
        OutputLastWin32Error("Failed to create Win32 window!");
    wfe::console::OutMessageFunction("Created Win32 window successfully.");
    
    // Show and update the window
    ShowWindow(hWnd, SW_SHOWNORMAL);
    UpdateWindow(hWnd);
}

// Windows functions
int main(int argc, char** args) {
    hInstance = GetModuleHandle(NULL);

    for(int32_t i = 0; i < argc; ++i) {
        if(!strcmp(args[i], "--vkdebug"))
            wfe::editor::EnableValidationLayers();
        else if(!strcmp(args[i], "--novkdebug"))
            wfe::editor::DisableValidationLayers();
    }
    wfe::console::OpenLogFile();
    
    RegisterWindowClass();
    CreateHWnd();

    MSG msg;
    while(GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (wfe::int32_t)msg.wParam;
}
LRESULT CALLBACK WinProc(_In_ HWND hWindow, _In_ UINT msg, _In_ WPARAM wParam, _In_ LPARAM lParam) {
    switch(msg) {
    case WM_CREATE:
        hWnd = hWindow;

        // Configure ImGui
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

        ImGui::GetIO().BackendPlatformName = "WFE-Win32-Backend";
        ImGui::GetIO().BackendFlags |= ImGuiBackendFlags_HasMouseCursors;

        ImGui::GetIO().LogFilename = "imgui-log.txt";
        ImGui::GetIO().IniFilename = nullptr;
        
        ImGui::GetIO().GetClipboardTextFn = GetClipboardText;
        ImGui::GetIO().SetClipboardTextFn = SetClipboardText;

        ImGui::StyleColorsDark();

        // Create all Vulkan objects
        wfe::editor::CreateDevice();
        wfe::editor::CreateSwapChain();
        wfe::editor::CreateImGuiPipeline();
        
        break;
    case WM_SIZE:
        windowWidth = (wfe::size_t)GET_X_LPARAM(lParam);
        windowHeight = (wfe::size_t)GET_Y_LPARAM(lParam);
        ImGui::GetIO().DisplaySize = { (wfe::float32_t)windowWidth, (wfe::float32_t)windowHeight };

        wfe::editor::RecreateSwapChain();
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

        // Process exceptions for shift, control and alt
        if(wParam == VK_SHIFT) {
            if(GetAsyncKeyState(VK_LSHIFT) >> 15) {
                ImGui::GetIO().AddKeyEvent(ImGuiKey_LeftShift, true);
                ImGui::GetIO().SetKeyEventNativeData(ImGuiKey_LeftShift, VK_LSHIFT, scancode);
            }
            if(GetAsyncKeyState(VK_RSHIFT) >> 15) {
                ImGui::GetIO().AddKeyEvent(ImGuiKey_RightShift, true);
                ImGui::GetIO().SetKeyEventNativeData(ImGuiKey_RightShift, VK_RSHIFT, scancode);
            }
        } else if(wParam == VK_CONTROL) {
            if(GetAsyncKeyState(VK_LCONTROL) >> 15) {
                ImGui::GetIO().AddKeyEvent(ImGuiKey_LeftCtrl, true);
                ImGui::GetIO().SetKeyEventNativeData(ImGuiKey_LeftCtrl, VK_LCONTROL, scancode);
            }
            if(GetAsyncKeyState(VK_RCONTROL) >> 15) {
                ImGui::GetIO().AddKeyEvent(ImGuiKey_RightCtrl, true);
                ImGui::GetIO().SetKeyEventNativeData(ImGuiKey_RightCtrl, VK_RCONTROL, scancode);
            }
        } else if(wParam == VK_MENU) {
            if(GetAsyncKeyState(VK_LMENU) >> 15) {
                ImGui::GetIO().AddKeyEvent(ImGuiKey_LeftSuper, true);
                ImGui::GetIO().SetKeyEventNativeData(ImGuiKey_LeftSuper, VK_LMENU, scancode);
            }
            if(GetAsyncKeyState(VK_RMENU) >> 15) {
                ImGui::GetIO().AddKeyEvent(ImGuiKey_RightSuper, true);
                ImGui::GetIO().SetKeyEventNativeData(ImGuiKey_RightSuper, VK_RMENU, scancode);
            }
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

        // Process exceptions for shift, control and alt
        if(wParam == VK_SHIFT) {
            if(!(GetAsyncKeyState(VK_LSHIFT) >> 15)) {
                ImGui::GetIO().AddKeyEvent(ImGuiKey_LeftShift, false);
                ImGui::GetIO().SetKeyEventNativeData(ImGuiKey_LeftShift, VK_LSHIFT, scancode);
            }
            if(!(GetAsyncKeyState(VK_RSHIFT) >> 15)) {
                ImGui::GetIO().AddKeyEvent(ImGuiKey_RightShift, false);
                ImGui::GetIO().SetKeyEventNativeData(ImGuiKey_RightShift, VK_RSHIFT, scancode);
            }
        } else if(wParam == VK_CONTROL) {
            if(!(GetAsyncKeyState(VK_LCONTROL) >> 15)) {
                ImGui::GetIO().AddKeyEvent(ImGuiKey_LeftCtrl, false);
                ImGui::GetIO().SetKeyEventNativeData(ImGuiKey_LeftCtrl, VK_LCONTROL, scancode);
            }
            if(!(GetAsyncKeyState(VK_RCONTROL) >> 15)) {
                ImGui::GetIO().AddKeyEvent(ImGuiKey_RightCtrl, false);
                ImGui::GetIO().SetKeyEventNativeData(ImGuiKey_RightCtrl, VK_RCONTROL, scancode);
            }
        } else if(wParam == VK_MENU) {
            if(!(GetAsyncKeyState(VK_LMENU) >> 15)) {
                ImGui::GetIO().AddKeyEvent(ImGuiKey_LeftSuper, false);
                ImGui::GetIO().SetKeyEventNativeData(ImGuiKey_LeftSuper, VK_LMENU, scancode);
            }
            if(!(GetAsyncKeyState(VK_RMENU) >> 15)) {
                ImGui::GetIO().AddKeyEvent(ImGuiKey_RightSuper, false);
                ImGui::GetIO().SetKeyEventNativeData(ImGuiKey_RightSuper, VK_RMENU, scancode);
            }
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
        wfe::editor::RenderWindows();
        wfe::editor::DrawImGui();
        return 0;
    case WM_CLOSE:
        DestroyWindow(hWnd);
        break;
    case WM_DESTROY:
        wfe::editor::SaveEditorProperties();
        wfe::editor::DeleteImGuiPipeline();

        ImGui::DestroyContext();

        wfe::editor::DeleteSwapChain();
        wfe::editor::DeleteDevice();

        wfe::console::OutMessageFunction("Deleted Win32 window successfully.");
        wfe::console::CloseLogFile();

        PostQuitMessage(0);
        break;
    }

    return DefWindowProc(hWindow, msg, wParam, lParam);
}

// External functions
wfe::size_t wfe::editor::GetMainWindowWidth() {
    return windowWidth;
}
wfe::size_t wfe::editor::GetMainWindowHeight() {
    return windowHeight;
}

const wfe::string& wfe::editor::GetMainWindowName() {
    return appName;
}
void wfe::editor::SetMainWindowName(const wfe::string& newName) {
    appName = newName;
    SetWindowText(hWnd, appName.c_str());
}

wfe::string wfe::editor::OpenFolderDialog(wfe::bool8_t& canceled, const string& startingLocation) {
    // Validate the window to avoid redrawing
    ValidateRect(hWnd, NULL);

    // Create the folder dialog
    IFileDialog* folderDialog;
    if(!SUCCEEDED(CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&folderDialog))))
        console::OutFatalError("Failed to create folder open dialog!", 1);
    
    // Set the folder dialog properties
    DWORD flags;
    folderDialog->GetOptions(&flags);
    flags |= FOS_PICKFOLDERS;
    folderDialog->SetOptions(flags);

    if(startingLocation.length()) { 
        // Convert the starting location from ASCII to wide characters
        size_t length = startingLocation.length();
        charw_t* startingLocationW = new charw_t[length + 1];

        mbstowcs(startingLocationW, startingLocation.c_str(), length);
        startingLocationW[length] = 0;

        // Create the PIDL
        PIDLIST_ABSOLUTE pidl;
        if(!SUCCEEDED(SHParseDisplayName(startingLocationW, NULL, &pidl, NULL, NULL)))
            console::OutFatalError("Failed to parse display name!", 1);

        // Create the start folder shell item
        IShellItem* startFolder;
        if(!SUCCEEDED(SHCreateShellItem(NULL, NULL, pidl, &startFolder)))
            console::OutFatalError("Failed to set start folder!", 1);
        
        // Set the folder dialog starting folder
        folderDialog->SetFolder(startFolder);

        ILFree(pidl);
    }

    // Show the folder dialog
    if(folderDialog->Show(hWnd) == HRESULT_FROM_WIN32(ERROR_CANCELLED)) {
        canceled = true;
        return "";
    } else
        canceled = false;
    
    // Get the results
    IShellItem* shellItem;
    if(!SUCCEEDED(folderDialog->GetResult(&shellItem)))
        console::OutFatalError("Failed to get folder dialog results!", 1);
    
    LPWSTR resultW;
    if(!SUCCEEDED(shellItem->GetDisplayName(SIGDN_DESKTOPABSOLUTEPARSING, &resultW)))
        console::OutFatalError("Failed to get folder dialog results!", 1);
    
    // Covert from wide string to ASCII
    size_t length = wcslen(resultW);
    string result;
    result.resize(length);

    wcstombs(result.data(), resultW, length);
    result[length] = 0;

    // Release the shell item and folder dialog
    shellItem->Release();
    folderDialog->Release();

    // Invalidate the window to reenable drawing
    InvalidateRect(hWnd, NULL, false);

    return result;
}

void wfe::editor::CopyFolder(const wfe::string& srcFolder, const wfe::string& dstFolder) {
    // Validate the window rectangle to avoid redrawind
    ValidateRect(hWnd, NULL);

    // Create a copy of the source and destination folders to add double null termination
    string srcFolderCopy = srcFolder;
    if(srcFolderCopy[srcFolderCopy.length() - 1] != '/' && srcFolderCopy[srcFolderCopy.length() - 1] != '\\')
        srcFolderCopy.append(1, '\\');
    srcFolderCopy.append(1, '*');
    srcFolderCopy.append(1, '\0');

    string dstFolderCopy = dstFolder;
    dstFolderCopy.append(1, '\0');
    
    // Set the copy info
    SHFILEOPSTRUCT copyInfo;

    copyInfo.hwnd = hWnd;
    copyInfo.wFunc = FO_COPY;
    copyInfo.pFrom = srcFolderCopy.c_str();
    copyInfo.pTo = dstFolderCopy.c_str();
    copyInfo.fFlags = FOF_SILENT;
    copyInfo.fAnyOperationsAborted = FALSE;
    copyInfo.hNameMappings = nullptr;
    copyInfo.lpszProgressTitle = nullptr;
    
    // Execute the file operation
    if(!SUCCEEDED(SHFileOperation(&copyInfo)))
        console::OutFatalError("Failed to copy files!", 1);

    // Invalidate the window rectangle to avoid redrawind
    InvalidateRect(hWnd, NULL, FALSE);
}

HWND wfe::editor::GetWindowHandle() {
    return hWnd;
}
HINSTANCE wfe::editor::GetWindowsInstance() {
    return hInstance;
}

#endif