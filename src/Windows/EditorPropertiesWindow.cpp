#include "Base/Window.hpp"

namespace wfe::editor {
    // Constants
    const size_t MAX_PATH_LENGTH = 256;

    // Variables
    string defaultTemplatePath;
    string defaultProjectPath;
    vector<string> templatePaths;

    // Public functions
    static void RenderWindow() {
        WindowType& windowType = WindowType::windowTypes["Editor Properties"];

        defaultTemplatePath.reserve(MAX_PATH_LENGTH);
        defaultProjectPath.reserve(MAX_PATH_LENGTH);

        if(ImGui::Begin("Editor Properties", &windowType.open)) {
            // Save the paths to buffers
            char_t defaultTemplatePathBuffer[MAX_PATH_LENGTH];
            strcpy(defaultTemplatePathBuffer, defaultTemplatePath.c_str());
            char_t defaultProjectPathBuffer[MAX_PATH_LENGTH];
            strcpy(defaultProjectPathBuffer, defaultProjectPath.c_str());

            // Display the default template path input
            ImGui::Text("Default template path");
            ImGui::SameLine();
            ImGui::InputText("", defaultTemplatePathBuffer, MAX_PATH_LENGTH);
            ImGui::SameLine();
            ImGui::Button("...");

            // Display the default project path input
            ImGui::Text("Default project path ");
            ImGui::SameLine();
            ImGui::InputText("", defaultProjectPathBuffer, MAX_PATH_LENGTH);
            ImGui::SameLine();
            ImGui::Button("...");

            // Copy from the buffers into the strings
            defaultTemplatePath = defaultTemplatePathBuffer;
            defaultProjectPath = defaultProjectPathBuffer;

            ImGui::End();
        }
    }
    string GetDefaultTemplatePath() {
        return defaultTemplatePath;
    }
    void SetDefaultTemplatePath(const string& newPath) {
        defaultTemplatePath = newPath;
    }
    string GetDefaultProjectPath() {
        return defaultProjectPath;
    }
    void SetDefaultProjectPath(const string& newPath) {
        defaultProjectPath = newPath;
    }

    WFE_EDITOR_WINDOW_TYPE("Editor Properties", RenderWindow)
}