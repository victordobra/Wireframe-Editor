#pragma once

#include "Core.hpp"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace wfe::editor {
    /// @brief Returns the width of the main window.
    size_t GetMainWindowWidth();
    /// @brief Returns the height of the main window.
    size_t GetMainWindowHeight();
    /// @brief Returns the X position of the main window.
    ptrdiff_t GetMainWindowXPos();
    /// @brief Returns the Y position of the main window.
    ptrdiff_t GetMainWindowYPos();

    /// @brief Returns the name of the application.
    string GetMainWindowName();
    /// @brief Sets the name of the application.
    /// @param newName The new name of the application.
    void SetMainWindowName(const string& newName);

    /// @brief Returns the handle of the window; meant for internal use.
    HWND GetMainWindowHandle();
    /// @brief Returns the handle of the Windows instance; meant for internal use.
    HINSTANCE GetWindowsInstance();
}