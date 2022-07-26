#pragma once

#include "Core.hpp"
#include "imgui.hpp"

namespace wfe::editor {
    struct WindowType {
        static map<string, WindowType> windowTypes;

        string name;
        void(*render)();
        bool8_t open;
    };

    void RenderWindows();
}

#define WFE_EDITOR_WINDOW_TYPE(winName, winRender) \
namespace { \
    /* Struct for creating the window type */ \
    struct EditorWindow ## type ## Init { \
        EditorWindow ## type ## Init() { \
            wfe::editor::WindowType windowType; \
 \
            windowType.name = winName; \
            windowType.render = winRender; \
            windowType.open = true; \
 \
            wfe::editor::WindowType::windowTypes.insert({ windowType.name, windowType }); \
        } \
    }; \
 \
    EditorWindow ## type ## Init init ## type; \
}