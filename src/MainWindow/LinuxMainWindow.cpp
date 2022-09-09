#include "BuildInfo.hpp"

#ifdef PLATFORM_LINUX

#include "MainWindow/LinuxMainWindow.hpp"
#include "LinuxToImGui.hpp"
#include "Core.hpp"
#include "Vulkan/Device.hpp"
#include "Vulkan/SwapChain.hpp"
#include "Vulkan/ImGuiPipeline.hpp"
#include "Base/Window.hpp"
#include "Windows/EditorPropertiesWindow.hpp"
#include "imgui.hpp"
#include "ProjectInfo.hpp"
#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <X11/keysym.h>

// Variables
Display* display;
wfe::int32_t screenIndex;
Screen* screen;

XIM inputMethod;
XIC inputContext;

Window window;
Atom wmDeleteMessage;
wfe::string windowName = PROJECT_NAME;
wfe::size_t windowWidth = 1000, windowHeight = 700;

wfe::bool8_t running = true;

// Internal helper functions
static void ConnectToXcb() {
    // Open the display
    display = XOpenDisplay(NULL);
    if(!display)
        wfe::console::OutFatalError("Failed to find display!", 1);

    // Get the default screen
    screenIndex = DefaultScreen(display);

    screen = XScreenOfDisplay(display, screenIndex);
}
static void CreateWindow() {
    // Load the editor properties
    wfe::editor::LoadEditorProperties();

    // Create the window
    window = XCreateSimpleWindow(display, screen->root, 0, 0, windowWidth, windowHeight, 0, screen->black_pixel, screen->black_pixel);

    // Select the event masks
    XSelectInput(display, window, KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask | ButtonMotionMask | StructureNotifyMask | ConfigureNotify);

    // Set the window's name
    windowName = wfe::editor::GetWorkspaceDir() + " - Wireframe Engine";
    XStoreName(display, window, windowName.c_str());

    // Set the window closing atom
    wmDeleteMessage = XInternAtom(display, "WM_DELETE_WINDOW", false);
    XSetWMProtocols(display, window, &wmDeleteMessage, 1);

    // Create the input method and context
    inputMethod = XOpenIM(display, NULL, NULL, NULL);
    inputContext = XCreateIC(inputMethod, XNInputStyle, XIMPreeditNothing | XIMStatusNothing, XNClientWindow, window, NULL);

    // Map the window
    XMapWindow(display, window);

    // Wait for the map notify event
    XEvent event;
    while(event.type != MapNotify)
        XNextEvent(display, &event);

    // Flush the display
    XFlush(display);
}
static void SetupImGui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::GetIO().BackendPlatformName = "WFE-Linux-Backend";

    ImGui::GetIO().LogFilename = "imgui-log.txt";
    ImGui::GetIO().IniFilename = nullptr;
    
    // TODO: Implement clipboard callbacks
    ImGui::GetIO().GetClipboardTextFn = nullptr;
    ImGui::GetIO().SetClipboardTextFn = nullptr;

    ImGui::GetIO().DisplaySize = { (wfe::float32_t)windowWidth, (wfe::float32_t)windowHeight };

    ImGui::StyleColorsDark();
}
static void ProcessEvent(const XEvent& event) {
    switch(event.type) {
    case KeyPress:
    {
        ImGuiKey key = KeySymToImGuiKey(XkbKeycodeToKeysym(display, event.xkey.keycode, 0, 0));
        ImGui::GetIO().AddKeyEvent(key, true);

        wfe::char_t buffer[32];
        Xutf8LookupString(inputContext, (XKeyEvent*)&event.xkey, buffer, 32, nullptr, nullptr);
        ImGui::GetIO().AddInputCharactersUTF8(buffer);

        break;
    }
    case KeyRelease:
    {
        ImGuiKey key = KeySymToImGuiKey(XkbKeycodeToKeysym(display, event.xkey.keycode, 0, 0));
        ImGui::GetIO().AddKeyEvent(key, false);
        break;
    }
    case ButtonPress:
    {
        ImGuiMouseButton mouseButton = XButtonToImGuiMouseButton(event.xbutton.button);

        // Weird workaround for scrollwheel input
        if(mouseButton == ImGuiMouseButton_COUNT) {
            // I hate X11
            switch(event.xbutton.button) {
            case 4:
                ImGui::GetIO().AddMouseWheelEvent(0.f, .5f);
                break;
            case 5:
                ImGui::GetIO().AddMouseWheelEvent(0.f, -.5f);
                break;
            }
        } else
            ImGui::GetIO().AddMouseButtonEvent(mouseButton, true);

        break;
    }
    case ButtonRelease:
    {
        ImGuiMouseButton mouseButton = XButtonToImGuiMouseButton(event.xbutton.button);
        if(mouseButton != ImGuiMouseButton_COUNT)
            ImGui::GetIO().AddMouseButtonEvent(mouseButton, false);

        break;
    }
    case MotionNotify:
        ImGui::GetIO().AddMousePosEvent((wfe::float32_t)event.xmotion.x, (wfe::float32_t)event.xmotion.y);

        break;
    case ConfigureNotify:
    {
        if(event.xconfigure.width != windowWidth || event.xconfigure.height != windowHeight) {
            windowWidth = event.xconfigure.width;
            windowHeight = event.xconfigure.height;

            ImGui::GetIO().DisplaySize = { (wfe::float32_t)windowWidth, (wfe::float32_t)windowHeight };

            wfe::editor::RecreateSwapChain();
        }

        break;
    }
    case ClientMessage:
        if(event.xclient.data.l[0] == wmDeleteMessage) {
            wfe::editor::SaveEditorProperties();
            running = false;
        }
        break;
    }
}

int main(int argc, char** args) {
    // Open the log file
    wfe::console::OpenLogFile();

    // Check for any special args
    for(int32_t i = 1; i < argc; ++i) {
        if(!strcmp(args[i], "--vkdebug"))
            wfe::editor::EnableValidationLayers();
        else if(!strcmp(args[i], "--novkdebug"))
            wfe::editor::DisableValidationLayers();
    }

    // Connect to X and create the window
    ConnectToXcb();
    CreateWindow();

    // Configure ImGui
    SetupImGui();

    // Create all Vulkan objects
    wfe::editor::CreateDevice();
    wfe::editor::CreateSwapChain();
    wfe::editor::CreateImGuiPipeline();

    XEvent event;
    while(running) {
        while(XPending(display)) {
            XNextEvent(display, &event);
            ProcessEvent(event);
        }

        wfe::editor::RenderWindows();
        wfe::editor::DrawImGui();
    }

    // Destroy the rest of the engine
    wfe::editor::DeleteImGuiPipeline();
    wfe::editor::DeleteSwapChain();
    wfe::editor::DeleteDevice();

    ImGui::DestroyContext();

    wfe::console::CloseLogFile();

    return 0;
}

// Public functions
wfe::size_t wfe::editor::GetMainWindowWidth() {
    return windowWidth;
}
wfe::size_t wfe::editor::GetMainWindowHeight() {
    return windowHeight;
}

wfe::string wfe::editor::GetMainWindowName() {
    return windowName;
}
void wfe::editor::SetMainWindowName(const wfe::string& newName) {
    windowName = newName;
    XStoreName(display, window, windowName.c_str());
}

wfe::string wfe::editor::OpenFolderDialog(wfe::bool8_t& canceled, const wfe::string& startingLocation) {
    // Create a command
    string fullCommand = (string)"zenity --file-selection --directory --filename=" + startingLocation;
    FILE* stream = popen(fullCommand.c_str(), "r");

    // Read the folder location
    char_t buffer[512];
    fgets(buffer, 512, stream);

    if(WEXITSTATUS(pclose(stream))) {
        canceled = true;
        return "";
    }

    // Return the final location
    string location = buffer;
    location[location.length() - 1] = 0;

    if(location[location.length() - 1] != '/')
        location.push_back('/');

    return location;
}
void wfe::editor::CopyFolder(const wfe::string& srcFolder, const wfe::string& dstFolder) {
    // Modify the source path
    string srcFolderMod = srcFolder;
    srcFolderMod.push_back('.');
    
    // Create a command
    string fullCommand = (string)"cp -a " + srcFolderMod + " " + dstFolder;
    FILE* stream = popen(fullCommand.c_str(), "r");
    pclose(stream);
}

Display* wfe::editor::GetScreenConnection() {
    return display;
}
Window wfe::editor::GetWindowHandle() {
    return window;
}

#endif