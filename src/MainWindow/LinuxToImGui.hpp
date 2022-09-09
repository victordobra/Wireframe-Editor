#pragma once

#include "Core.hpp"
#include "imgui.hpp"
#include <xcb/xcb.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>

ImGuiKey KeySymToImGuiKey(KeySym keySym);
ImGuiMouseButton XButtonToImGuiMouseButton(wfe::uint32_t button);
