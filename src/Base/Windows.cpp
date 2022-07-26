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
        ImGui::NewFrame();
        for(auto* window : windows)
            window->RenderWindow();
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