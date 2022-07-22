#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "imgui.hpp"
#include "Core.hpp"

ImGuiKey Win32KeyToImGuiKey(wfe::int32_t key);
LPTSTR ImGuiCursorToWin32CursorType(ImGuiMouseCursor cursor);