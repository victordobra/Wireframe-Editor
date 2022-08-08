#pragma once

#include "Core.hpp"
#include <windows.h>

namespace wfe::editor {
    /// @brief Returns the main window's height.
    size_t GetMainWindowWidth();
    /// @brief Returns the main window's width.
    size_t GetMainWindowHeight();

    /// @brief Returns the main window's name.
    string GetMainWindowName();
    /// @brief Set the main window's name.
    void SetMainWindowName(const string& newName);

    /// @brief Creates a open folder name dialog.
    /// @param canceled A reference to a bool which will be set to true if the dialog was canceled.
    /// @return The absolute paht of the selected folder.
    string OpenFolderDialog(bool8_t& canceled, const string& startingLocation = "");

    /// @brief Returns the window's handle. Meant for internal use.
    HWND GetWindowHandle();
    /// @brief Returns the Windows instance's handle. Meant for internal use.
    HINSTANCE GetWindowsInstance();
}