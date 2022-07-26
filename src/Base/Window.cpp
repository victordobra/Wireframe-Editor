#include "Base/Window.hpp"

namespace wfe::editor {
    // Static variables
    constinit map<string, WindowType> WindowType::windowTypes;

    // Public functions
    void RenderWindows() {
        // Begin the frame
        ImGui::NewFrame();

        // Create the dock space
        ImGui::SetNextWindowViewport(ImGui::GetMainViewport()->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

        ImGuiWindowFlags dockSpaceFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        ImGui::Begin("DockSpace", nullptr, dockSpaceFlags);

        ImGui::SetWindowPos(ImGui::GetMainViewport()->WorkPos);
        ImGui::SetWindowSize(ImGui::GetMainViewport()->WorkSize);

        ImGui::PopStyleVar(2);

        ImGuiID dockSpaceID = ImGui::GetID("DockSpace");
        ImGui::DockSpace(dockSpaceID, ImVec2(0.f, 0.f), ImGuiDockNodeFlags_None);

        // Render every window
        for(auto& windowType : WindowType::windowTypes)
            if(windowType.val2.open)
                windowType.val2.render();

        ImGui::End();
        ImGui::Render();
    }
}