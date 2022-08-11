#include "Base/Window.hpp"
#include "MainWindow/MainWindow.hpp"

namespace wfe::editor {
    // Constants
    const size_t MAX_PATH_LENGTH = 256;

    // Variables
    string defaultTemplateLocation = "";
    string defaultProjectLocation = "";
    vector<string> templateLocations{};

    // Public functions
    static void RenderWindow() {
        WindowType& windowType = WindowType::windowTypes["Editor Properties"];

        if(ImGui::Begin("Editor Properties", &windowType.open)) {
            ImGui::PushItemWidth(-200.f);

            // Display the default template path dialog button
            if(ImGui::Button("...##defaultTemplateLocationButton")) {
                bool8_t canceled;
                string result = OpenFolderDialog(canceled);
                if(!canceled)
                    defaultTemplateLocation = result;
            }

            // Save the default template path to a buffer
            char_t defaultTemplateLocationBuffer[MAX_PATH_LENGTH];
            strcpy(defaultTemplateLocationBuffer, defaultTemplateLocation.c_str());

            // Display the default template path input
            ImGui::SameLine();
            ImGui::InputText("Default template path##defaultTemplateLocationInput", defaultTemplateLocationBuffer, MAX_PATH_LENGTH);
            defaultTemplateLocation = defaultTemplateLocationBuffer;

            // Display the default project path dialog buffer
            if(ImGui::Button("...##defaultProjectLocationButton")) {
                bool8_t canceled;
                string result = OpenFolderDialog(canceled);
                if(!canceled)
                    defaultProjectLocation = result;
            }

            // Save the default project path to a buffer
            char_t defaultProjectLocationBuffer[MAX_PATH_LENGTH];
            strcpy(defaultProjectLocationBuffer, defaultProjectLocation.c_str());
            
            // Display the default project path input
            ImGui::SameLine();
            ImGui::InputText("Default project path##defaultProjectLocationInput", defaultProjectLocationBuffer, MAX_PATH_LENGTH);
            defaultProjectLocation = defaultProjectLocationBuffer;

            // Display the list of templates
            if(ImGui::BeginListBox("Templates##templatesListBox", ImVec2(0.f, 200.f))) {
                ImGui::PushItemWidth(-.01f);
                for(size_t i = 0; i < templateLocations.size(); ++i) {
                    // Display the path remove button
                    if(ImGui::Button(((string)"X##templateLocationRemoveButton" + ToString(i)).c_str())) {
                        // Remove the current path from the list
                        templateLocations.erase(templateLocations.begin() + i);
                        --i;
                        continue;
                    }

                    // Display the template path dialog button
                    ImGui::SameLine();
                    if(ImGui::Button(((string)"...##templateLocationButton" + ToString(i)).c_str())) {
                        bool8_t canceled;
                        string result = OpenFolderDialog(canceled, defaultTemplateLocation);
                        if(!canceled)
                            templateLocations[i] = result;
                    }

                    // Put the template path into a buffer
                    char_t buffer[MAX_PATH_LENGTH];
                    strcpy(buffer, templateLocations[i].c_str());

                    // Display the template path input
                    ImGui::SameLine();
                    ImGui::InputText(((string)"##templateLocationInput" + ToString(i)).c_str(), buffer, MAX_PATH_LENGTH);
                    templateLocations[i] = buffer;
                }
                ImGui::PopItemWidth();
                ImGui::EndListBox();
            }
            
            // Add button to add templates
            if(ImGui::Button("+##templateAddButton", ImVec2(-200.f, 0.f))) {
                templateLocations.push_back("");
            }

            ImGui::PopItemWidth();
            ImGui::End();
        }
    }

    
    void LoadEditorProperties() {
        FileInput input{ "editor.properties" };

        if(!input)
            return;
        
        // Input the default template and project paths
        input.ReadLine(defaultTemplateLocation, defaultTemplateLocation.max_size());
        input.ReadLine(defaultProjectLocation, defaultProjectLocation.max_size());

        // Input every template path
        while(!input.IsAtTheEnd()) {
            string line;
            input.ReadLine(line, line.max_size());
            if(line.length())
                templateLocations.push_back(line);
        }

        input.Close();
    }
    void SaveEditorProperties() {
        FileOutput output{ "editor.properties" };

        // Output the default template and project paths
        output.Write(defaultTemplateLocation); output.WriteBuffer('\n');
        output.Write(defaultProjectLocation);  output.WriteBuffer('\n');

        // Output every template path
        for(const auto& templateLocation : templateLocations) {
            output.Write(templateLocation); output.WriteBuffer('\n');
        }

        output.Close();
    }

    string GetDefaultTemplateLocation() {
        return defaultTemplateLocation;
    }
    void SetDefaultTemplateLocation(const string& newLocation) {
        defaultTemplateLocation = newLocation;
    }
    string GetDefaultProjectLocation() {
        return defaultProjectLocation;
    }
    void SetDefaultProjectLocation(const string& newLocation) {
        defaultProjectLocation = newLocation;
    }
    vector<string> GetTemplateLocations() {
        return templateLocations;
    }

    WFE_EDITOR_WINDOW_TYPE("Editor Properties", RenderWindow)
}