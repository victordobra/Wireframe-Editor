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

        // Start the menu bar
        if(ImGui::BeginMenuBar()) {
            // Display basic file settings
            if(ImGui::BeginMenu("File")) {
                if(ImGui::MenuItem("New project", "Ctrl+N")) {
                    // TODO: Creating a new project
                    console::OutMessageFunction("New project");
                }
                if(ImGui::MenuItem("Open project", "Ctrl+O")) {
                    // TODO: Opening a project
                    console::OutMessageFunction("Loaded project");
                }
                if(ImGui::MenuItem("Save", "Ctrl+S")) {
                    // TODO: Saving callback
                    console::OutMessageFunction("Saved project");
                }
                if(ImGui::MenuItem("Exit", nullptr)) {
                    // Exit the program
                    exit(0);
                }
                
                ImGui::EndMenu();
            }
            // Display every window type
            if(ImGui::BeginMenu("Windows")) {
                for(auto& windowType : WindowType::windowTypes) {
                    ImGui::MenuItem(windowType.val2.name.c_str(), nullptr, &windowType.val2.open);
                }
                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }

        // Render every window
        for(auto& windowType : WindowType::windowTypes)
            if(windowType.val2.open)
                windowType.val2.render();

        ImGui::End();
        ImGui::Render();
    }
}