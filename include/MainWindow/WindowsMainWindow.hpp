#pragma once

#include "Core.hpp"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

namespace wfe::editor {
    size_t GetMainWindowWidth();
    size_t GetMainWindowHeight();

    HWND GetWindowHandle();
    HINSTANCE GetWindowsInstance();
}