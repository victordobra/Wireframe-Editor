#pragma once

#include "Core.hpp"

namespace wfe::editor {
    void LoadColors();
    void SaveColors();

    uint32_t GetBackgroundColor();
    uint32_t GetEdgeColor();
    uint32_t GetForegroundColor();
    uint32_t GetHighlightColor();
    uint32_t GetTextColor();
    uint32_t GetDockingColor();

    void SetBackgroundColor(uint32_t newColor);
    void SetEdgeColor(uint32_t newColor);
    void SetForegroundColor(uint32_t newColor);
    void SetHighlightColor(uint32_t newColor);
    void SetTextColor(uint32_t newColor);
    void SetDockingColor(uint32_t newColor);
}