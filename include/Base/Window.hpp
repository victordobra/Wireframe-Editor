#pragma once

#include "Core.hpp"
#include "imgui.hpp"

namespace wfe::editor {
    class Window {
    public:
        Window();
        Window(const Window&) = delete;
        Window(Window&&) noexcept = delete;

        Window& operator=(const Window&) = delete;
        Window& operator=(Window&&) noexcept = delete;

        virtual void RenderWindow();

        static void RenderWindows();

        ~Window();
    private:
        static vector<Window*> windows;
    };
}