#include "Base/Window.hpp"
#include "MainWindow/MainWindow.hpp"
#include "Windows/EditorPropertiesWindow.hpp"
#include "Windows/NewProjectWindow.hpp"

namespace wfe::editor {
    // Variables
    constinit map<string, WindowType> WindowType::windowTypes;

    constinit LoadCallback loadCallback = 0;
    constinit SaveCallback saveCallback = 0;

    string workspaceDir = "";
    vector<string> recentDirs;

    // Public functions
    void LoadWorkspace() {
        FileInput input("recent.info");

        if(input) {
            while(!input.IsAtTheEnd()) {
                string line;
                input.ReadLine(line, line.max_size());
                if(line.length())
                    recentDirs.push_back(line);
            }

            input.Close();
        }

        // Set the workspace dir as the most recent dir (if it exists)
        if(recentDirs.size())
            workspaceDir = recentDirs[0];
    }
    void SaveWorkspace() {
        FileOutput output("recent.info");

        for(const auto& recentDir : recentDirs) {
            output.Write(recentDir);
            output.WriteBuffer('\n');
        }

        output.Close();
    }

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

        // Handle the new project shortcut
        if(ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_N, false)) {
            // Create a new project
            CreateNewProject();
        }
        // Handle the open project shortcut
        if(ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_O, false)) {
            // TODO: Add saving safety check

            // Open a dialog to get the project location
            bool8_t canceled;
            string result = OpenFolderDialog(canceled, GetDefaultProjectLocation());
            
            if(!canceled) {
                // Set the workspace dir
                SetWorkspaceDir(result);

                // Call the load callback, if it exists
                if(loadCallback)
                    loadCallback();
            }
        }
        // Handle the save project shortcut
        if(ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_O, false)) { 
            // Call the save callback, if it exists
            if(saveCallback)
                saveCallback();
        }

        // Start the menu bar
        if(ImGui::BeginMenuBar()) {
            // Display basic file settings
            if(ImGui::BeginMenu("File")) {
                if(ImGui::MenuItem("New project", "Ctrl+N")) {
                    // Create a new project
                    CreateNewProject();
                }

                ImGui::Separator();
                if(ImGui::MenuItem("Open project", "Ctrl+O")) {
                    // TODO: Add saving safety check

                    // Open a dialog to get the project location
                    bool8_t canceled;
                    string result = OpenFolderDialog(canceled, GetDefaultProjectLocation());
                    
                    if(!canceled) {
                        // Set the workspace dir
                        SetWorkspaceDir(result);

                        // Call the load callback, if it exists
                        if(loadCallback)
                            loadCallback();
                    }
                }
                if(ImGui::BeginMenu("Open recent")) {
                    for(size_t i = 0; i < recentDirs.size(); ++i)
                        if(ImGui::MenuItem(recentDirs[i].c_str(), nullptr) && i) {
                            // TODO: Check if the path still exists
                            // TODO: Add saving safety check

                            // Put the recent dir at the top
                            string recentDir = recentDirs[i];
                            recentDirs.erase(recentDirs.data() + i);
                            
                            // Set the workspace dir
                            SetWorkspaceDir(recentDir, false);

                            // Call the save callback, if it exists
                            if(saveCallback)
                                saveCallback();
                        }
                    
                    // Add an option to clear all recent dirs
                    ImGui::Separator();
                    if(ImGui::MenuItem("Clear recents", nullptr)) {
                        recentDirs.clear();
                        SaveWorkspace();
                    }

                    ImGui::EndMenu();
                }

                ImGui::Separator();

                if(ImGui::MenuItem("Save", "Ctrl+S")) {
                    // Call the save callback, if it exists
                    if(saveCallback)
                        saveCallback();
                }

                ImGui::Separator();

                if(ImGui::MenuItem("Exit", nullptr)) {
                    // Exit the program
                    exit(0);
                }
                
                ImGui::EndMenu();
            }
            // Display every window type
            if(ImGui::BeginMenu("Windows")) {
                for(auto& windowType : WindowType::windowTypes)
                    ImGui::MenuItem(windowType.val2.name.c_str(), nullptr, &windowType.val2.open);
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

    LoadCallback GetLoadCallback() {
        return loadCallback;
    }
    void SetLoadCallback(LoadCallback newLoadCallback) {
        loadCallback = newLoadCallback;
    }
    SaveCallback GetSaveCallback() {
        return saveCallback;
    }
    void SetSaveCallback(SaveCallback newSaveCallback) {
        saveCallback = newSaveCallback;
    }

    string GetWorkspaceDir() {
        return workspaceDir;
    }
    void SetWorkspaceDir(const string& newWorkspaceDir, bool8_t removeFromRecents) {
        workspaceDir = newWorkspaceDir;

        // Remove from the recents list if needed
        if(removeFromRecents)
            for(const auto& recentDir : recentDirs)
                if(recentDir == workspaceDir) {
                    recentDirs.erase(&recentDir);
                    break;
                }
        // Place as the first in the recent dir list
        recentDirs.insert(recentDirs.begin(), workspaceDir);

        // Save the recent dirs
        SaveWorkspace();

        // Set the main window name
        SetMainWindowName(workspaceDir + " - Wireframe Engine");
    }
}