#include "Base/Window.hpp"
#include "MainWindow/MainWindow.hpp"

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

            // Display the default template path dialog button
            if(ImGui::Button("...##defaultTemplatePathButton")) {
                bool8_t canceled;
                string result = OpenFolderDialog(canceled);
                if(!canceled)
                    defaultTemplatePath = result;
            }

            // Save the default template path to a buffer
            char_t defaultTemplatePathBuffer[MAX_PATH_LENGTH];
            strcpy(defaultTemplatePathBuffer, defaultTemplatePath.c_str());

            // Display the default template paht input
            ImGui::SameLine();
            ImGui::InputText("Default template path##defaultTemplatePathInput", defaultTemplatePathBuffer, MAX_PATH_LENGTH);
            defaultTemplatePath = defaultTemplatePathBuffer;

            // Display the default project path dialog buffer
            if(ImGui::Button("...##defaultProjectPathButton")) {
                bool8_t canceled;
                string result = OpenFolderDialog(canceled);
                if(!canceled)
                    defaultProjectPath = result;
            }

            // Save the default project path to a buffer
            char_t defaultProjectPathBuffer[MAX_PATH_LENGTH];
            strcpy(defaultProjectPathBuffer, defaultProjectPath.c_str());
            
            // Display the default project path input
            ImGui::SameLine();
            ImGui::InputText("Default project path##defaultProjectPathInput", defaultProjectPathBuffer, MAX_PATH_LENGTH);
            defaultProjectPath = defaultProjectPathBuffer;

            // Display the list of templates
            if(ImGui::BeginListBox("Templates##templatesListBox", ImVec2(0.f, 200.f))) {
                ImGui::PushItemWidth(-.01f);
                for(size_t i = 0; i < templatePaths.size(); ++i) {
                    // Display the path remove button
                    if(ImGui::Button(((string)"X##templatePathRemoveButton" + ToString(i)).c_str())) {
                        // Remove the current path from the list
                        templatePaths.erase(templatePaths.begin() + i);
                        --i;
                        continue;
                    }

                    // Display the template path dialog button
                    ImGui::SameLine();
                    if(ImGui::Button(((string)"...##templatePathButton" + ToString(i)).c_str())) {
                        bool8_t canceled;
                        string result = OpenFolderDialog(canceled, defaultTemplatePath);
                        if(!canceled)
                            templatePaths[i] = result;
                    }

                    // Put the template path into a buffer
                    char_t buffer[MAX_PATH_LENGTH];
                    strcpy(buffer, templatePaths[i].c_str());

                    // Display the template path input
                    ImGui::SameLine();
                    ImGui::InputText(((string)"##templatePathInput" + ToString(i)).c_str(), buffer, MAX_PATH_LENGTH);
                    templatePaths[i] = buffer;
                }
                ImGui::PopItemWidth();
                ImGui::EndListBox();
            }
            
            // Add button to add templates
            if(ImGui::Button("+##templateAddButton", ImVec2(-200.f, 0.f))) {
                templatePaths.push_back("");
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
        input.ReadLine(defaultTemplatePath, defaultTemplatePath.max_size());
        input.ReadLine(defaultProjectPath, defaultProjectPath.max_size());

        // Input every template path
        while(!input.IsAtTheEnd()) {
            string line;
            input.ReadLine(line, line.max_size());
            if(line.length())
                templatePaths.push_back(line);
        }

        input.Close();
    }
    void SaveEditorProperties() {
        FileOutput output{ "editor.properties" };

        // Output the default template and project paths
        output.Write(defaultTemplatePath); output.WriteBuffer('\n');
        output.Write(defaultProjectPath);  output.WriteBuffer('\n');

        // Output every template path
        for(const auto& templatePath : templatePaths) {
            output.Write(templatePath); output.WriteBuffer('\n');
        }

        output.Close();
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
    vector<string> GetTemplatePaths() {
        return templatePaths;
    }

    WFE_EDITOR_WINDOW_TYPE("Editor Properties", RenderWindow)
}