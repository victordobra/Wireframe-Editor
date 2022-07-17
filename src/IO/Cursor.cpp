#include "IO/Cursor.hpp"
#include "MainWindow/MainWindow.hpp"

#ifdef PLATFORM_WINDOWS
#include <windows.h>
#endif

namespace wfe::editor {
    bool8_t cursorDown = false, cursorPressed = false, cursorReleased = false;
    CursorType cursorType = CURSOR_TYPE_DEFAULT;

    // OS specific functions
#ifdef PLATFORM_WINDOWS
    HCURSOR cursor = nullptr;

    void UpdateInput() {
        cursorType = CURSOR_TYPE_DEFAULT;

        bool8_t newCursorDown = GetAsyncKeyState(VK_LBUTTON) >> 15;
        cursorPressed = !cursorDown && newCursorDown;
        cursorReleased = cursorDown && !newCursorDown;
        cursorDown = newCursorDown;
    }

    CursorPos GetCursorPos() {
        POINT cursorPoint;
        WINBOOL result = ::GetCursorPos(&cursorPoint);

        if(!result) {
            char_t error[256];
            FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), error, 256 * sizeof(char_t), NULL);
            console::OutFatalError((string)"Failed to obtain cursor position! Error: " + error, 1);
        }

        result = ScreenToClient(GetMainWindowHandle(), &cursorPoint);

        if(!result) {
            char_t error[256];
            FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), error, 256 * sizeof(char_t), NULL);
            console::OutFatalError((string)"Failed to convert from screen pos to client pos! Error: " + error, 1);
        }

        CursorPos cursorPos{ (ptrdiff_t)cursorPoint.x, (ptrdiff_t)cursorPoint.y };
        return cursorPos;
    }
    CursorPos GetCursorScreenPos() {
        POINT cursorPoint;
        WINBOOL result = ::GetCursorPos(&cursorPoint);

        if(!result) {
            char_t error[256];
            FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), error, 256 * sizeof(char_t), NULL);
            console::OutFatalError((string)"Failed to obtain cursor position! Error: " + error, 1);
        }

        CursorPos cursorPos{ (ptrdiff_t)cursorPoint.x, (ptrdiff_t)cursorPoint.y };
        return cursorPos;
    }

    void SetCursorPos(CursorPos newPos) {
        POINT cursorPoint{ (int32_t)newPos.x, (int32_t)newPos.y };

        WINBOOL result = ClientToScreen(GetMainWindowHandle(), &cursorPoint);

        if(!result) {
            char_t error[256];
            FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), error, 256 * sizeof(char_t), NULL);
            console::OutFatalError((string)"Failed to convert from client pos to screen pos! Error: " + error, 1);
        }

        result = ::SetCursorPos(cursorPoint.x, cursorPoint.y);

        if(!result) {
            char_t error[256];
            FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), error, 256 * sizeof(char_t), NULL);
            console::OutFatalError((string)"Failed to set cursor position! Error: " + error, 1);
        }
    }
    void SetCursorScreenPos(CursorPos newPos) {
        WINBOOL result = ::SetCursorPos((int32_t)newPos.x, (int32_t)newPos.y);

        if(!result) {
            char_t error[256];
            FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), error, 256 * sizeof(char_t), NULL);
            console::OutFatalError((string)"Failed to set cursor position! Error: " + error, 1);
        }
    }

    bool8_t CursorDown() {
        return cursorDown;
    }
    bool8_t CursorPressed() {
        return cursorPressed;
    }
    bool8_t CursorReleased() {
        return cursorReleased;
    }

    void UpdateCursorType() {
        // If the cursor type is CURSOR_TYPE_DEFAULT, let the OS determine the cursor's type
        if(cursorType == CURSOR_TYPE_DEFAULT)
            return;
        
        // Delete the old cursor if it exists
        delete cursor;

        // Convert the engine's cursor type to Windows' cursor type
        LPCSTR windowsCursorType;

        switch(cursorType) {
        case CURSOR_TYPE_ARROW:
            windowsCursorType = IDC_ARROW;
            break;
        case CURSOR_TYPE_CROSS:
            windowsCursorType = IDC_CROSS;
            break;
        case CURSOR_TYPE_HAND:
            windowsCursorType = IDC_HAND;
            break;
        case CURSOR_TYPE_NO:
            windowsCursorType = IDC_NO;
            break;
        case CURSOR_TYPE_SIZE_ALL:
            windowsCursorType = IDC_SIZEALL;
            break;
        case CURSOR_TYPE_SIZE_UPDOWN:
            windowsCursorType = IDC_SIZENS;
            break;
        case CURSOR_TYPE_SIZE_LEFTRIGHT:
            windowsCursorType = IDC_SIZEWE;
            break;
        case CURSOR_TYPE_SIZE_DIAGONAL_LEFT:
            windowsCursorType = IDC_SIZENWSE;
            break;
        case CURSOR_TYPE_SIZE_DIAGONAL_RIGHT:
            windowsCursorType = IDC_SIZENESW;
            break;
        case CURSOR_TYPE_WAIT:
            windowsCursorType = IDC_WAIT;
            break;
        default:
            windowsCursorType = IDC_ARROW;
            break;
        }

        // Load the cursor
        cursor = LoadCursor(NULL, windowsCursorType);
        
        if(!cursor) {
            char_t error[256];
            FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), error, 256 * sizeof(char_t), NULL);
            console::OutFatalError((string)"Failed to load the cursor! Error: " + error, 1);
        }

        // Set the new cursor
        cursor = SetCursor(cursor);

        if(!cursor) {
            char_t error[256];
            FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), error, 256 * sizeof(char_t), NULL);
            console::OutFatalError((string)"Failed to set the cursor! Error: " + error, 1);
        }
    }
#endif

    // Other functions
    CursorType GetCursorType() {
        return cursorType;
    }
    void SetCursorType(CursorType newType) {
        cursorType = newType;
    }
}