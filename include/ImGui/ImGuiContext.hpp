#pragma once

#include "imgui.hpp"

namespace wfe::editor {
    void CreateImGuiContext();
    void DeleteImGuiContext();

    ImGuiContext* GetImGuiContext();
    ImGuiIO* GetImGuiIO();
    ImGuiStyle* GetImGuiStyle();
}