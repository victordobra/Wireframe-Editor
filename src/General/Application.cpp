#include "General/Application.hpp"
#include "EditorPlatform/Platform.hpp"
#include "ProjectInfo.hpp"

namespace wfe::editor {
    // Variables
    vector<Event> eventQueue[EVENT_TYPE_COUNT]; // Stores every event
    vector<Event> temporaryEventQueue;          // Stores every event sent by the user program

    bool8_t running = true;           // Whether the program is running
    bool8_t renderingWindows = false; // Whether the windows are currently rendering

    size_t mainWindowWidth = DEFAULT_WINDOW_WIDTH;   // The width of the main window
    size_t mainWindowHeight = DEFAULT_WINDOW_HEIGHT; // The height of the main window
    string mainWindowName = WFE_EDITOR_PROJECT_NAME; // The name of the main window

    // Internal helper functions

    // Processes all useful events
    static int32_t ProcessEvents() {
        // Check if there are any exit events
        if(eventQueue[EVENT_TYPE_EXIT].size()) {
            // The app is not running anymore; set running to false
            running = false;

            // Return the first non-zero return code
            for(auto& exitEvent : eventQueue[EVENT_TYPE_EXIT]) {
                ExitEventInfo* exitEventInfo = (ExitEventInfo*)exitEvent.eventInfo;

                if(exitEventInfo->returnCode)
                    return exitEventInfo->returnCode;
            }

            // No non-zero return code was found; simply return 0
            return 0;
        }

        // Check if there are any resize events
        if(eventQueue[EVENT_TYPE_WINDOW_RESIZE].size()) {
            // Set the size of the window to the size of the last resize event
            auto& lastResizeEvent = eventQueue[EVENT_TYPE_WINDOW_RESIZE].back();
            WindowResizeEventInfo* resizeEventInfo = (WindowResizeEventInfo*)lastResizeEvent.eventInfo;

            mainWindowWidth = resizeEventInfo->windowWidth;
            mainWindowHeight = resizeEventInfo->windowHeight;

            RecreateSwapChain();
        }

        // Check if there are any rename events
        if(eventQueue[EVENT_TYPE_WINDOW_RENAME].size()) {
            // Set the name of the window to the name of the last rename event
            auto& lastRenameEvent = eventQueue[EVENT_TYPE_WINDOW_RENAME].back();
            WindowRenameEventInfo* renameEventInfo = (WindowRenameEventInfo*)lastRenameEvent.eventInfo;

            mainWindowName = renameEventInfo->newName;
        }

        return 0;
    }
    // Removes all events
    static void RemoveAllEvents() {
        // Remove all events from the main queues
        for(size_t i = 0; i < EVENT_TYPE_COUNT; ++i) {
            for(auto& event : eventQueue[i])
                free(event.eventInfo);

            eventQueue[i].clear();
        }
        
        // Add all events from the temporary event queue to the main event queues
        for(auto& event : temporaryEventQueue)
            eventQueue[event.eventType].push_back(event);
        
        // Clear the temporary event queue
        temporaryEventQueue.clear();
    }

    int main(int argc, char** args) {
        // Check for evety console arg
        for(int32_t i = 1; i < argc; ++i) {
            if(!strcmp(args[i], "--vkdebug")) {
                EnableValidationLayers();
            } else if(!strcmp(args[i], "--novkdebug")) {
                DisableValidationLayers();
            }
        }
        
        // Create everything
        console::OpenLogFile();

        CreateImGui();
        CreatePlatform();
        CreateDevice();
        CreateSwapChain();
        CreateImGuiPipeline();

        LoadEditorProperties();
        LoadWorkspace();

        // Main message loop
        int32_t returnCode;

        while(running) {
            // Poll events
            PollPlatformEvents();

            // Process the polled events
            returnCode = ProcessEvents();
            if(!running)
                break;

            ProcessImGuiEvents();

            // Start rendering the windows
            renderingWindows = true;

            RenderWindows();
            DrawImGui();

            // Stop rendering the windows
            renderingWindows = false;

            // Remove all events from the queue
            RemoveAllEvents();
        }

        // Delete everything
        DeleteImGuiPipeline();
        DeleteSwapChain();
        DeleteDevice();
        DeletePlatform();
        DeleteImGui();

        console::CloseLogFile();

        return returnCode;
    }

    // Public functions
    vector<Event> GetEvents() {
        // Copy every event queue's contents into a vector
        vector<Event> events;

        for(size_t i = 0; i < EVENT_TYPE_COUNT; ++i)
            events.insert(events.end(), eventQueue[i].begin(), eventQueue[i].end());
        
        return events;
    }
    vector<Event> GetEventsOfType(EventType type) {
        return eventQueue[type];
    }

    void AddEvent(Event event) {
        if(renderingWindows) {
            // Add the event to the temporary event queue
            temporaryEventQueue.push_back(event);
        } else {
            // Add the event to the coresponding queue
            eventQueue[event.eventType].push_back(event);
        }
    }

    void CloseApplication(int32_t returnCode) {
        // Run any functions that should be run before the platform shuts down
        SaveEditorProperties();
        CloseWorkspace();

        // Create an EVENT_TYPE_EXIT event
        ExitEventInfo* exitEventInfo = new ExitEventInfo();
        exitEventInfo->returnCode = returnCode;

        Event event;
        event.eventType = EVENT_TYPE_EXIT;
        event.eventInfo = &exitEventInfo;

        // Add the event to the queue
        AddEvent(event);
    }
    bool8_t IsInsideEditor() {
        return true;
    }

    size_t GetMainWindowWidth() {
        return mainWindowWidth;
    }
    size_t GetMainWindowHeight() {
        return mainWindowHeight;
    }

    string GetMainWindowName() {
        return mainWindowName;
    }
    void SetMainWindowName(const string& newName) {
        // Set the new window name
        mainWindowName = newName;

        // Create a rename event
        WindowRenameEventInfo* renameEventInfo = new WindowRenameEventInfo();
        renameEventInfo->newName = mainWindowName;

        Event renameEvent;
        renameEvent.eventType = EVENT_TYPE_WINDOW_RENAME;
        renameEvent.eventInfo = renameEventInfo;

        // Add the rename event to the event queue
        AddEvent(renameEvent);
    }
}

int main(int argc, char** args) {
    return wfe::editor::main(argc, args);
}