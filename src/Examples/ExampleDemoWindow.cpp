#include "Base/Window.hpp"

namespace wfe::editor {
    void RenderWindow() {
        bool8_t windowOpen = true;
        if(ImGui::Begin("Example Demo Window", &windowOpen)) {
            char_t buffer[256];

            ImGui::Text("Test");
            ImGui::InputText("Text input", buffer, 256);
            ImGui::Button("Button");

            ImGui::End();
        }

        WindowType::windowTypes["Example Demo Window"].open = windowOpen;
    }

    WFE_EDITOR_WINDOW_TYPE("Example Demo Window", RenderWindow)
}