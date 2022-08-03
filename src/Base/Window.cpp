#include "Base/Window.hpp"
#include "MainWindow/MainWindow.hpp"

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

        // Start the menu bar
        if(ImGui::BeginMenuBar()) {
            // Display basic file settings
            if(ImGui::BeginMenu("File")) {
                if(ImGui::MenuItem("New project", "Ctrl+N")) {
                    // TODO: Creating a new project
                    console::OutMessageFunction("New project");
                }

                ImGui::Separator();

                if(ImGui::MenuItem("Open project", "Ctrl+O")) {
                    // TODO: Create a load popup

                    if(loadCallback)
                        loadCallback();

                    console::OutMessageFunction("Loaded project");
                }
                if(ImGui::BeginMenu("Open recent")) {
                    for(size_t i = 1; i < recentDirs.size(); ++i)
                        if(ImGui::MenuItem(recentDirs[i].c_str(), nullptr)) {
                            // TODO: Check if the path still exists

                            // Put the recent dir at the top
                            string recentDir = recentDirs[i];
                            recentDirs.erase(recentDirs.data() + i);
                            
                            // Set the workspace dir
                            SetWorkspaceDir(recentDir, false);

                            if(saveCallback)
                                saveCallback();
                        }
                    
                    ImGui::EndMenu();
                }

                ImGui::Separator();

                if(ImGui::MenuItem("Save", "Ctrl+S")) {
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