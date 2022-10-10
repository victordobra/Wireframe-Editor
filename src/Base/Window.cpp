#include "Base/Window.hpp"
#include "General/Application.hpp"
#include "EditorPlatform/Platform.hpp"
#include "Windows/EditorPropertiesWindow.hpp"
#include "Windows/NewProjectWindow.hpp"
#include "Linking/FunctionPtrs.hpp"
#include <dlfcn.h>

namespace wfe::editor {
    // Variables
    constinit map<string, WindowType> windowTypesInternal;

    constinit map<string, WindowType>* WindowType::windowTypes = &windowTypesInternal;

    constinit LoadCallback loadCallback;
    constinit SaveCallback saveCallback;
    constinit CloseCallback closeCallback;

    string workspaceDir = "";
    vector<string> recentDirs;
    void* workspaceDynamicLib;

    // Internal helper functions
    static void LoadEditorInfo() {
        // Load the ImGui ini settings
        ImGui::LoadIniSettingsFromDisk((workspaceDir + "imgui.ini").c_str());

        // Load the editor info        
        FileInput fileInput(workspaceDir + "editor.info");

        if(!fileInput)
            return;
        
        // Get the number of window types
        size_t windowTypeCount = 0;
        fileInput.Read(windowTypeCount);

        // Read every window type
        for(size_t i = 0; i < windowTypeCount; ++i){
            string windowTypeName = "";
            size_t windowTypeOpen = 0;

            fileInput.Read(windowTypeOpen).ReadLine(windowTypeName, windowTypeName.max_size());

            // Check if it exists
            auto* pair = WindowType::windowTypes->find(windowTypeName);
            if(pair != WindowType::windowTypes->end()) {
                // Set whether it is open
                pair->val2.open = windowTypeOpen;
            }
        }

        fileInput.Close();
    }
    static void SaveEditorInfo() {
        // Save the ImGui ini settings
        ImGui::SaveIniSettingsToDisk((workspaceDir + "imgui.ini").c_str());

        // Save the editor info
        FileOutput fileOutput(workspaceDir + "editor.info");

        // Write the number of window types
        fileOutput.Write(WindowType::windowTypes->size()).WriteBuffer('\n');

        for(auto& pair : *WindowType::windowTypes) {
            fileOutput.Write(pair.val2.name).WriteBuffer(' ').Write((size_t)pair.val2.open).WriteBuffer('\n');
        }

        fileOutput.Close();
    }

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
            SetWorkspaceDir(recentDirs[0]);
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
            string result = OpenFileDialog("Open Project", canceled, GetDefaultProjectLocation(), true);
            
            if(!canceled) {
                // Set the workspace dir
                SetWorkspaceDir(result);
            }
        }
        // Handle the save project shortcut
        if(ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_S, false)) { 
            // Save the editor info
            SaveEditorInfo();

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
                    string result = OpenFileDialog("Open Project", canceled, GetDefaultProjectLocation(), true);
                    
                    if(!canceled) {
                        // Set the workspace dir
                        SetWorkspaceDir(result);
                    }
                }
                if(ImGui::BeginMenu("Open recent")) {
                    for(size_t i = 0; i < recentDirs.size(); ++i)
                        if(ImGui::MenuItem(recentDirs[i].c_str(), nullptr) && i) {
                            // Check if the recent location still exists
                            if(!LocationExists(recentDirs[i])) {
                                ImGui::OpenPopup("Project loocation doesn't exist!");
                            }
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

                if(ImGui::BeginPopupModal("Project loocation doesn't exist!")) {
                    ImGui::Text("The specified project does not exist. Removed from recents.");
                    if(ImGui::Button("OK", ImVec2(-0.1f, 0.f)))
                        ImGui::CloseCurrentPopup();
                    ImGui::EndPopup();
                }

                ImGui::Separator();

                if(ImGui::MenuItem("Save", "Ctrl+S")) {
                    // Save the editor info
                    SaveEditorInfo();

                    // Call the save callback, if it exists
                    if(saveCallback)
                        saveCallback();
                }

                ImGui::Separator();

                if(ImGui::MenuItem("Close", nullptr)) {
                    // TODO: Add saving safety check

                    CloseWorkspace();
                }
                
                ImGui::EndMenu();
            }
            // Display every window type
            if(ImGui::BeginMenu("Windows")) {
                for(auto& windowType : *WindowType::windowTypes)
                    ImGui::MenuItem(windowType.val2.name.c_str(), nullptr, &windowType.val2.open);
                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }

        // Render every window
        for(auto& windowType : *WindowType::windowTypes)
            if(windowType.val2.open)
                windowType.val2.render();

        ImGui::PopStyleVar(2);

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
    CloseCallback GetCloseCallback() {
        return closeCallback;
    }
    void SetCloseCallback(CloseCallback newCloseCallback) {
        closeCallback = newCloseCallback;
    }

    string GetWorkspaceDir() {
        return workspaceDir;
    }
    void SetWorkspaceDir(const string& newWorkspaceDir, bool8_t removeFromRecents) {
        CloseWorkspace();

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

        // Link the project's dynamic library
#if defined(PLATFORM_WINDOWS)
        string workspaceDynamicLibPath = workspaceDir + "build/libEditorLib.dll";
#elif defined(PLATFORM_LINUX)
        string workspaceDynamicLibPath = workspaceDir + "build/libEditorLib.so";
#endif
        workspaceDynamicLib = dlopen(workspaceDynamicLibPath.c_str(), RTLD_LAZY);

        // Set all callbacks
        using SetCallbacks = void(*)(const EditorCallbacks&);
        SetCallbacks setCallbacks = (SetCallbacks)dlsym(workspaceDynamicLib, "SetCallbacks");

        EditorCallbacks callbacks;
        GenerateEditorCallbacks(callbacks, &windowTypesInternal);

        if(setCallbacks)
            setCallbacks(callbacks);

        // Call the load callback, if it exists
        if(loadCallback)
            loadCallback();
        
        LoadEditorInfo();

        console::OutMessageFunction((string)"Opened " + workspaceDir);
    }
    void CloseWorkspace() {
        if(workspaceDynamicLib) {
            // Call the close callback, if it exists
            if(closeCallback)
                closeCallback();
            
            // Reset all callback values
            loadCallback = nullptr;
            saveCallback = nullptr;
            closeCallback = nullptr;

            // Store all messages
            vector<console::Message> messages = console::GetMessages();

            // Close the dynamic lib
            dlclose(workspaceDynamicLib);
            workspaceDynamicLib = nullptr;

            // Set the pointer in the message vector to a nullptr, should modify this
            *((void**)&console::GetMessages() + 2) = nullptr;
            console::GetMessages() = messages;

            // Close and reopen the log file in case it was already closed
            console::OpenLogFile();

            // Remove all windows except the main editor windows
            pair<string, WindowType>* ptr = WindowType::windowTypes->begin();
            while(WindowType::windowTypes->size() > 2)
                if(ptr->val1 != "Editor Properties" && ptr->val1 != "New Project")
                    WindowType::windowTypes->erase(ptr);
                else
                    ++ptr;
            
            console::OutMessageFunction((string)"Closed " + workspaceDir);
            workspaceDir = "";

            SetMainWindowName("Wireframe Engine");
        }
    }
}