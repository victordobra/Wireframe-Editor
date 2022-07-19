#include "ImGui/ImGuiContext.hpp"
#include "ImGui/ImGuiPipeline.hpp"

namespace wfe::editor {
    // Variables
    ImGuiContext* context;
    ImGuiIO* io;
    ImGuiStyle* style;

    // Functions
    void CreateImGuiContext() {
        // Create the context
        context = ImGui::CreateContext();

        // Configure IO
        io = &ImGui::GetIO();
        io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

        // Style ImGui
        ImGui::StyleColorsDark();

        style = &ImGui::GetStyle();

        style->WindowRounding = 0.0f;
        style->Colors[ImGuiCol_WindowBg].w = 1.0f;

        // Create the ImGui pipeline
        CreateImGuiPipeline();
    }
    void DeleteImGuiContext() {
        // Delete the ImGui pipeline
        DeleteImGuiPipeline();

        // Delete the context
        ImGui::DestroyContext(context);
    }

    ImGuiContext* GetImGuiContext() {
        return context;
    }
    ImGuiIO* GetImGuiIO() {
        return io;
    }
    ImGuiStyle* GetImGuiStyle() {
        return style;
    }
}