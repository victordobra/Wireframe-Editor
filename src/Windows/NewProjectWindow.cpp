#include "Windows/NewProjectWindow.hpp"
#include "Windows/EditorPropertiesWindow.hpp"
#include "Base/Window.hpp"
#include "EditorPlatform/Platform.hpp"

namespace wfe::editor {
    // Constants
    const size_t MAX_PATH_LENGTH = 256;

    // Variables
    string projectLocation = "";
    int32_t templateIndex = -1;

    // Public functions
    static void RenderWindow() {
        WindowType& windowType = WindowType::windowTypes->at("New Project");

        ImGui::SetNextWindowSize(ImVec2(500.f, 200.f), ImGuiCond_FirstUseEver);
        if(ImGui::Begin("New Project", &windowType.open)) {
            ImGui::PushItemWidth(-200.f);

            // Display the project path button
            if(ImGui::Button("...##projectPathButton")) {
                // Open a dialog to get the project location
                bool8_t canceled;
                string result = OpenFileDialog("Select Project Location", canceled, GetDefaultProjectLocation(), true);
                
                if(!canceled)
                    projectLocation = result;
            }

            // Copy the project path to a buffer
            char_t projectLocationBuffer[MAX_PATH_LENGTH];
            strcpy(projectLocationBuffer, projectLocation.c_str());

            // Display the project path input
            ImGui::SameLine();
            ImGui::InputText("Project location", projectLocationBuffer, MAX_PATH_LENGTH);
            projectLocation = projectLocationBuffer;

            // Add every template to a vector
            vector<string> templateStrings = GetTemplateLocations();
            vector<const char_t*> templates;

            for(const auto& templateString : templateStrings)
                templates.push_back(templateString.c_str());
            
            // Display the template list
            ImGui::ListBox("Template", &templateIndex, templates.data(), (int32_t)templates.size());

            // Display the create button
            if(ImGui::Button("Create Project")) {
                // Copy from the template path to the project path
                CopyFiles(templateStrings[templateIndex], projectLocation, true);

                // Set the workspace dir
                SetWorkspaceDir(projectLocation);
                
                // Reset all project info
                windowType.open = false;
                projectLocation = "";
                templateIndex = 0;
            }
            
            ImGui::PopItemWidth();

            ImGui::End();
        }
    }
    void CreateNewProject() {
        WindowType::windowTypes->at("New Project").open = true;
    }

    WFE_EDITOR_WINDOW_TYPE("New Project", RenderWindow)
}