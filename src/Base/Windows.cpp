#include "Base/Window.hpp"

namespace wfe::editor {
    // Static variable
    vector<Window*> Window::windows;

    // Public member functions
    Window::Window() {
        windows.push_back(this);
    }

    void Window::RenderWindow() {

    }
    void Window::RenderWindows() {
        // Begin the frame
        ImGui::NewFrame();

        // Create the dock space
        ImGui::SetNextWindowPos(ImGui::GetMainViewport()->WorkPos);
        ImGui::SetNextWindowSize(ImGui::GetMainViewport()->WorkSize);
        ImGui::SetNextWindowViewport(ImGui::GetMainViewport()->ID);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

        ImGuiWindowFlags dockSpaceFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        ImGui::Begin("DockSpace", nullptr, dockSpaceFlags);

        ImGui::PopStyleVar(2);

        ImGuiID dockSpaceID = ImGui::GetID("DockSpace");
        ImGui::DockSpace(dockSpaceID, ImVec2(0.f, 0.f), ImGuiDockNodeFlags_None);

        for(auto* window : windows)
            window->RenderWindow();

        ImGui::End();
        ImGui::Render();
    }

    Window::~Window() {
        for(auto*& window : windows)
            if(window == this) {
                windows.erase(&window);
                break;
            }
    }
}