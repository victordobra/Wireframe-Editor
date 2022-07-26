#pragma once

#include "Base/Window.hpp"

namespace wfe::editor {
    class ExampleDemoWindow : public Window {
    public:
        ExampleDemoWindow() = default;
        ExampleDemoWindow(const ExampleDemoWindow&) = delete;
        ExampleDemoWindow(ExampleDemoWindow&&) noexcept = delete;

        ExampleDemoWindow& operator=(const ExampleDemoWindow&) = delete;
        ExampleDemoWindow& operator=(ExampleDemoWindow&&) noexcept = delete;

        void RenderWindow() override {
            bool8_t open;
            ImGui::Begin("Demo Window", &open);

            char_t buffer[256];

            ImGui::Text("Test");
            ImGui::InputText("Text input", buffer, 256);
            ImGui::Button("Button");

            ImGui::End();
        }

        ~ExampleDemoWindow() = default;
    };
}