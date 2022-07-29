#include "Base/Window.hpp"

namespace wfe::editor {
    // Constants
    const size_t MAX_PATH_LENGTH = 256;

    // Variables
    string defaultTemplatePath = "";
    string defaultProjectPath = "";
    vector<string> templatePaths{};

    // Public functions
    static void RenderWindow() {
        WindowType& windowType = WindowType::windowTypes["Editor Properties"];

        if(ImGui::Begin("Editor Properties", &windowType.open)) {
            ImGui::PushItemWidth(-200.f);

            // Save the paths to buffers
            char_t defaultTemplatePathBuffer[MAX_PATH_LENGTH];
            strcpy(defaultTemplatePathBuffer, defaultTemplatePath.c_str());
            char_t defaultProjectPathBuffer[MAX_PATH_LENGTH];
            strcpy(defaultProjectPathBuffer, defaultProjectPath.c_str());

            // Display the default template path input
            if(ImGui::Button("...##defaultTemplatePathButton")) {
                // TODO: Add path browse window
            }

            ImGui::SameLine();
            ImGui::InputText("Default template path##defaultTemplatePathInput", defaultTemplatePathBuffer, MAX_PATH_LENGTH);

            // Display the default project path input
            if(ImGui::Button("...##defaultProjectPathButton")) {
                // TODO: Add path browse window
            }
            ImGui::SameLine();
            ImGui::InputText("Default project path##defaultProjectPathInput", defaultProjectPathBuffer, MAX_PATH_LENGTH);

            // Copy from the buffers into the strings
            defaultTemplatePath = defaultTemplatePathBuffer;
            defaultProjectPath = defaultProjectPathBuffer;

            // Display the list of templates
            if(ImGui::BeginListBox("Templates##templatesListBox", ImVec2(0.f, 200.f))) {
                ImGui::PushItemWidth(-75.f);
                for(size_t i = 0; i < templatePaths.size(); ++i) {
                    // Copy the template string to a buffer
                    char_t buffer[MAX_PATH_LENGTH];
                    strcpy(buffer, templatePaths[i].c_str());

                    // Display the path text input
                    ImGui::InputText(((string)"##templatePathInput" + ToString(i)).c_str(), buffer, MAX_PATH_LENGTH);
                    templatePaths[i] = buffer;
                    
                    ImGui::SameLine();
                    if(ImGui::Button(((string)"...##templatePathButton" + ToString(i)).c_str())) {
                        // TODO: Add path browse window
                    }

                    ImGui::SameLine();
                    if(ImGui::Button(((string)"X##templatePathRemoveButton" + ToString(i)).c_str())) {
                        // Remove the current path from the list
                        templatePaths.erase(templatePaths.begin() + i);
                        --i;
                    }
                }
                ImGui::PopItemWidth();
                ImGui::EndListBox();
            }
            // Add button to add templates
            if(ImGui::Button("+##templateAddButton", ImVec2(ImGui::GetWindowWidth() - 17.5f, 0.f))) {
                templatePaths.push_back("");
            }

            ImGui::PopItemWidth();
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