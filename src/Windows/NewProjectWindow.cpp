#include "Windows/NewProjectWindow.hpp"
#include "Windows/EditorPropertiesWindow.hpp"
#include "Base/Window.hpp"
#include "MainWindow/MainWindow.hpp"

namespace wfe::editor {
    // Constants
    const size_t MAX_PATH_LENGTH = 256;

    // Variables
    string projectLocation = "";
    int32_t templateIndex = -1;

    // Public functions
    static void RenderWindow() {
        WindowType& windowType = WindowType::windowTypes["New Project"];

        if(ImGui::Begin("New Project", &windowType.open)) {
            // Display the project path button
            if(ImGui::Button("...##projectPathButton")) {
                // Open a dialog to get the project location
                bool8_t canceled;
                string result = OpenFolderDialog(canceled, GetDefaultProjectLocation());
                
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
                CopyFolder(templateStrings[templateIndex], projectLocation);

                // Set the workspace dir
                SetWorkspaceDir(projectLocation);

                // Call the load callback, if it exists
                if(GetLoadCallback())
                    GetLoadCallback()();
                
                // Reset all project info
                windowType.open = false;
                projectLocation = "";
                templateIndex = 0;
            }
            ImGui::End();
        }
    }
    void CreateNewProject() {
        WindowType::windowTypes["New Project"].open = true;
    }

    WFE_EDITOR_WINDOW_TYPE("New Project", RenderWindow)
}