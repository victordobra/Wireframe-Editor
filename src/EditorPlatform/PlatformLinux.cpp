#include "BuildInfo.hpp"

#ifdef PLATFORM_LINUX

#include "EditorPlatform/Platform.hpp"
#include "General/Application.hpp"
#include "Core.hpp"
#include "ProjectInfo.hpp"
#include "imgui.hpp"
#include <sys/stat.h>
#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <X11/keysym.h>

// Constants
const wfe::size_t SCROLL_WHEEL_DELTA = .5f;

// Variables
wfe::editor::PlatformInfo editorPlatformInfo;

// Internal helper functions
// Converts the Xkb KeySym to an ImGui key code
static ImGuiKey KeySymToImGuiKey(KeySym keySym) {
    switch(keySym) {
    case XK_Tab: 
        return ImGuiKey_Tab;
    case XK_Left: 
        return ImGuiKey_LeftArrow;
    case XK_Right: 
        return ImGuiKey_RightArrow;
    case XK_Up: 
        return ImGuiKey_UpArrow;
    case XK_Down: 
        return ImGuiKey_DownArrow;
    case XK_Page_Up: 
        return ImGuiKey_PageUp;
    case XK_Page_Down: 
        return ImGuiKey_PageDown;
    case XK_Home: 
        return ImGuiKey_Home;
    case XK_End: 
        return ImGuiKey_End;
    case XK_Insert: 
        return ImGuiKey_Insert;
    case XK_Delete: 
        return ImGuiKey_Delete;
    case XK_BackSpace: 
        return ImGuiKey_Backspace;
    case XK_space: 
        return ImGuiKey_Space;
    case XK_Return: 
        return ImGuiKey_Enter;
    case XK_Escape: 
        return ImGuiKey_Escape;
    case XK_Shift_L:
        return ImGuiKey_LeftShift;
    case XK_Shift_R:
        return ImGuiKey_RightShift;
    case XK_Control_L:
        return ImGuiKey_LeftCtrl;
    case XK_Control_R:
        return ImGuiKey_RightCtrl;
    case XK_Alt_L:
        return ImGuiKey_LeftAlt;
    case XK_Alt_R:
        return ImGuiKey_RightAlt;
    case XK_apostrophe: 
        return ImGuiKey_Apostrophe;
    case XK_comma: 
        return ImGuiKey_Comma;
    case XK_minus: 
        return ImGuiKey_Minus;
    case XK_period: 
        return ImGuiKey_Period;
    case XK_slash: 
        return ImGuiKey_Slash;
    case XK_semicolon: 
        return ImGuiKey_Semicolon;
    case XK_equal: 
        return ImGuiKey_Equal;
    case XK_bracketleft: 
        return ImGuiKey_LeftBracket;
    case XK_backslash: 
        return ImGuiKey_Backslash;
    case XK_bracketright: 
        return ImGuiKey_RightBracket;
    case XK_grave: 
        return ImGuiKey_GraveAccent;
    case XK_Caps_Lock: 
        return ImGuiKey_CapsLock;
    case XK_Scroll_Lock: 
        return ImGuiKey_ScrollLock;
    case XK_Num_Lock: 
        return ImGuiKey_NumLock;
    case XK_Pause: 
        return ImGuiKey_Pause;
    case XK_KP_0: 
        return ImGuiKey_Keypad0;
    case XK_KP_1: 
        return ImGuiKey_Keypad1;
    case XK_KP_2: 
        return ImGuiKey_Keypad2;
    case XK_KP_3: 
        return ImGuiKey_Keypad3;
    case XK_KP_4: 
        return ImGuiKey_Keypad4;
    case XK_KP_5: 
        return ImGuiKey_Keypad5;
    case XK_KP_6: 
        return ImGuiKey_Keypad6;
    case XK_KP_7: 
        return ImGuiKey_Keypad7;
    case XK_KP_8: 
        return ImGuiKey_Keypad8;
    case XK_KP_9: 
        return ImGuiKey_Keypad9;
    case XK_KP_Decimal: 
        return ImGuiKey_KeypadDecimal;
    case XK_KP_Divide: 
        return ImGuiKey_KeypadDivide;
    case XK_KP_Multiply: 
        return ImGuiKey_KeypadMultiply;
    case XK_KP_Subtract: 
        return ImGuiKey_KeypadSubtract;
    case XK_KP_Add: 
        return ImGuiKey_KeypadAdd;
    case XK_Super_L: 
        return ImGuiKey_LeftSuper;
    case XK_Super_R: 
        return ImGuiKey_RightSuper;
    case XK_Menu: 
        return ImGuiKey_Menu;
    case XK_0: 
        return ImGuiKey_0;
    case XK_1: 
        return ImGuiKey_1;
    case XK_2: 
        return ImGuiKey_2;
    case XK_3:
        return ImGuiKey_3;
    case XK_4:
        return ImGuiKey_4;
    case XK_5:
        return ImGuiKey_5;
    case XK_6:
        return ImGuiKey_6;
    case XK_7:
        return ImGuiKey_7;
    case XK_8:
        return ImGuiKey_8;
    case XK_9:
        return ImGuiKey_9;
    case XK_A:
    case XK_a:
        return ImGuiKey_A;
    case XK_b:
    case XK_B:
        return ImGuiKey_B;
    case XK_c:
    case XK_C:
        return ImGuiKey_C;
    case XK_d:
    case XK_D:
        return ImGuiKey_D;
    case XK_e:
    case XK_E:
        return ImGuiKey_E;
    case XK_f:
    case XK_F:
        return ImGuiKey_F;
    case XK_g:
    case XK_G:
        return ImGuiKey_G;
    case XK_h:
    case XK_H:
        return ImGuiKey_H;
    case XK_i:
    case XK_I:
        return ImGuiKey_I;
    case XK_j:
    case XK_J:
        return ImGuiKey_J;
    case XK_k:
    case XK_K:
        return ImGuiKey_K;
    case XK_l:
    case XK_L:
        return ImGuiKey_L;
    case XK_m:
    case XK_M:
        return ImGuiKey_M;
    case XK_n:
    case XK_N:
        return ImGuiKey_N;
    case XK_o:
    case XK_O:
        return ImGuiKey_O;
    case XK_p:
    case XK_P:
        return ImGuiKey_P;
    case XK_q:
    case XK_Q:
        return ImGuiKey_Q;
    case XK_r:
    case XK_R:
        return ImGuiKey_R;
    case XK_s:
    case XK_S:
        return ImGuiKey_S;
    case XK_t:
    case XK_T:
        return ImGuiKey_T;
    case XK_u:
    case XK_U:
        return ImGuiKey_U;
    case XK_v:
    case XK_V:
        return ImGuiKey_V;
    case XK_w:
    case XK_W:
        return ImGuiKey_W;
    case XK_x:
    case XK_X:
        return ImGuiKey_X;
    case XK_y:
    case XK_Y:
        return ImGuiKey_Y;
    case XK_z:
    case XK_Z:
        return ImGuiKey_Z;
    case XK_F1: 
        return ImGuiKey_F1;
    case XK_F2: 
        return ImGuiKey_F2;
    case XK_F3: 
        return ImGuiKey_F3;
    case XK_F4: 
        return ImGuiKey_F4;
    case XK_F5: 
        return ImGuiKey_F5;
    case XK_F6: 
        return ImGuiKey_F6;
    case XK_F7: 
        return ImGuiKey_F7;
    case XK_F8: 
        return ImGuiKey_F8;
    case XK_F9: 
        return ImGuiKey_F9;
    case XK_F10: 
        return ImGuiKey_F10;
    case XK_F11: 
        return ImGuiKey_F11;
    case XK_F12: 
        return ImGuiKey_F12;
    default: 
        return ImGuiKey_None;
    }
}
// Convert the X mouse button index to an ImGui mouse button
static ImGuiMouseButton XButtonToImGuiMouseButton(wfe::uint32_t button) {
    switch(button) {
    case 1:
        return ImGuiMouseButton_Left;
    case 2:
        return ImGuiMouseButton_Middle;
    case 3:
        return ImGuiMouseButton_Right;
    default:
        return ImGuiMouseButton_COUNT;
    }
}

// Connects to X
static void ConnectToX() {
    // Open the display
    editorPlatformInfo.display = XOpenDisplay(NULL);
    if(!editorPlatformInfo.display)
        wfe::console::OutFatalError("Failed to find display!", 1);

    // Get the default screen
    editorPlatformInfo.screenIndex = DefaultScreen(editorPlatformInfo.display);

    editorPlatformInfo.screen = XScreenOfDisplay(editorPlatformInfo.display, editorPlatformInfo.screenIndex);
}
// Creates the main window
static void CreateWindow() {
    // Create the window
    editorPlatformInfo.window = XCreateSimpleWindow(editorPlatformInfo.display, editorPlatformInfo.screen->root, 0, 0, wfe::editor::DEFAULT_WINDOW_WIDTH, wfe::editor::DEFAULT_WINDOW_HEIGHT, 0, editorPlatformInfo.screen->black_pixel, editorPlatformInfo.screen->black_pixel);

    // Set the window's name
    XStoreName(editorPlatformInfo.display, editorPlatformInfo.window, WFE_EDITOR_PROJECT_NAME);

    // Select the event masks
    XSelectInput(editorPlatformInfo.display, editorPlatformInfo.window, KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask | ButtonMotionMask | StructureNotifyMask | ConfigureNotify);

    // Set the window closing atom
    editorPlatformInfo.deleteMessageAtom = XInternAtom(editorPlatformInfo.display, "WM_DELETE_WINDOW", false);
    XSetWMProtocols(editorPlatformInfo.display, editorPlatformInfo.window, &editorPlatformInfo.deleteMessageAtom, 1);

    // Create the input method and context
    editorPlatformInfo.inputMethod = XOpenIM(editorPlatformInfo.display, NULL, NULL, NULL);
    editorPlatformInfo.inputContext = XCreateIC(editorPlatformInfo.inputMethod, XNInputStyle, XIMPreeditNothing | XIMStatusNothing, XNClientWindow, editorPlatformInfo.window, NULL);

    // Map the window
    XMapWindow(editorPlatformInfo.display, editorPlatformInfo.window);

    // Wait for the map notify event
    XEvent event;
    while(event.type != MapNotify)
        XNextEvent(editorPlatformInfo.display, &event);

    // Flush the display
    XFlush(editorPlatformInfo.display);
}
static void ProcessEvent(const XEvent& event) {
    switch(event.type) {
    case KeyPress:
    {
        // Get the pressed ImGui key
        ImGuiKey key = KeySymToImGuiKey(XkbKeycodeToKeysym(editorPlatformInfo.display, event.xkey.keycode, 0, 0));

        // Create the key event
        wfe::editor::KeyEventInfo* keyEventInfo = new wfe::editor::KeyEventInfo();
        keyEventInfo->pressed = true;
        keyEventInfo->keyCode = key;

        wfe::editor::Event keyEvent;
        keyEvent.eventType = wfe::editor::EVENT_TYPE_KEY;
        keyEvent.eventInfo = keyEventInfo;

        // Add the key event to the queue
        wfe::editor::AddEvent(keyEvent);

        // Get the inputted UTF8 string
        wfe::char_t buffer[32];
        Xutf8LookupString(editorPlatformInfo.inputContext, (XKeyPressedEvent*)&event.xkey, buffer, 32, nullptr, nullptr);

        // Create the input char event
        wfe::editor::InputCharEventInfo* inputCharEventInfo = new wfe::editor::InputCharEventInfo();
        inputCharEventInfo->inputString = buffer;
        
        wfe::editor::Event inputCharEvent;
        inputCharEvent.eventType = wfe::editor::EVENT_TYPE_INPUT_CHAR;
        inputCharEvent.eventInfo = inputCharEventInfo;

        // Add the input char event to the queue
        wfe::editor::AddEvent(inputCharEvent);

        break;
    }
    case KeyRelease:
    {
        // Get the released ImGui key
        ImGuiKey key = KeySymToImGuiKey(XkbKeycodeToKeysym(editorPlatformInfo.display, event.xkey.keycode, 0, 0));

        // Create the key event
        wfe::editor::KeyEventInfo* keyEventInfo = new wfe::editor::KeyEventInfo();
        keyEventInfo->pressed = false;
        keyEventInfo->keyCode = key;

        wfe::editor::Event keyEvent;
        keyEvent.eventType = wfe::editor::EVENT_TYPE_KEY;
        keyEvent.eventInfo = keyEventInfo;

        // Add the key event to the queue
        wfe::editor::AddEvent(keyEvent);

        break;
    }
    case ButtonPress:
    {
        // Get the ImGui mouse button
        ImGuiMouseButton mouseButton = XButtonToImGuiMouseButton(event.xbutton.button);

        // Weird workaround for scrollwheel input
        if(mouseButton == ImGuiMouseButton_COUNT) {
            // Set the scroll wheel delta based on which button was pressed (?!)
            wfe::float32_t scrollWheelDelta;

            switch (event.xbutton.button) {
            case 4:
                scrollWheelDelta = SCROLL_WHEEL_DELTA;
                break;
            case 5:
                scrollWheelDelta = -SCROLL_WHEEL_DELTA;
                break;
            default:
                scrollWheelDelta = 0.f;
                break;
            }

            if(scrollWheelDelta != 0.f) {
                // Create the mouse move event
                wfe::editor::MouseMoveEventInfo* mouseMoveEventInfo = new wfe::editor::MouseMoveEventInfo();
                mouseMoveEventInfo->mouseX = -1;
                mouseMoveEventInfo->mouseY = -1;
                mouseMoveEventInfo->scrollWheelDelta = scrollWheelDelta;

                wfe::editor::Event mouseMoveEvent;
                mouseMoveEvent.eventType = wfe::editor::EVENT_TYPE_MOUSE_MOVE;
                mouseMoveEvent.eventInfo = mouseMoveEventInfo;

                // Add the mouse move event to the queue
                wfe::editor::AddEvent(mouseMoveEvent);
            }
        } else {
            // Create the mouse button event
            wfe::editor::MouseButtonEventInfo* mouseButtonEventInfo = new wfe::editor::MouseButtonEventInfo();
            mouseButtonEventInfo->pressed = true;
            mouseButtonEventInfo->mouseButton = mouseButton;

            wfe::editor::Event mouseButtonEvent;
            mouseButtonEvent.eventType = wfe::editor::EVENT_TYPE_MOUSE_BUTTON;
            mouseButtonEvent.eventInfo = mouseButtonEventInfo;

            // Add the mouse button event to the queue
            wfe::editor::AddEvent(mouseButtonEvent);
        }

        break;
    }
    case ButtonRelease:
    {
        // Get the ImGui mouse button
        ImGuiMouseButton mouseButton = XButtonToImGuiMouseButton(event.xbutton.button);

        if(mouseButton != ImGuiMouseButton_COUNT) {
            // Create the mouse button event
            wfe::editor::MouseButtonEventInfo* mouseButtonEventInfo = new wfe::editor::MouseButtonEventInfo();
            mouseButtonEventInfo->pressed = false;
            mouseButtonEventInfo->mouseButton = mouseButton;

            wfe::editor::Event mouseButtonEvent;
            mouseButtonEvent.eventType = wfe::editor::EVENT_TYPE_MOUSE_BUTTON;
            mouseButtonEvent.eventInfo = mouseButtonEventInfo;

            // Add the mouse button event to the queue
            wfe::editor::AddEvent(mouseButtonEvent);
        }

        break;
    }
    case MotionNotify:
    {
        // Create the mouse move event
        wfe::editor::MouseMoveEventInfo* mouseMoveEventInfo = new wfe::editor::MouseMoveEventInfo();
        mouseMoveEventInfo->mouseX = event.xmotion.x;
        mouseMoveEventInfo->mouseY = event.xmotion.y;
        mouseMoveEventInfo->scrollWheelDelta = 0.f;

        wfe::editor::Event mouseMoveEvent;
        mouseMoveEvent.eventType = wfe::editor::EVENT_TYPE_MOUSE_MOVE;
        mouseMoveEvent.eventInfo = mouseMoveEventInfo;

        // Add the mouse move event to the queue
        wfe::editor::AddEvent(mouseMoveEvent);

        break;
    }
    case ConfigureNotify:
    {
        // Create the window move event
        wfe::editor::WindowMoveEventInfo* windowMoveEventInfo = new wfe::editor::WindowMoveEventInfo();
        windowMoveEventInfo->windowX = event.xconfigure.x;
        windowMoveEventInfo->windowY = event.xconfigure.y;

        wfe::editor::Event windowMoveEvent;
        windowMoveEvent.eventType = wfe::editor::EVENT_TYPE_WINDOW_MOVE;
        windowMoveEvent.eventInfo = windowMoveEventInfo;

        // Add the window move event to the queue
        wfe::editor::AddEvent(windowMoveEvent);

        // Create the window resize event
        wfe::editor::WindowResizeEventInfo* windowResizeEventInfo = new wfe::editor::WindowResizeEventInfo();
        windowResizeEventInfo->windowWidth = event.xconfigure.width;
        windowResizeEventInfo->windowHeight = event.xconfigure.height;

        wfe::editor::Event windowResizeEvent;
        windowResizeEvent.eventType = wfe::editor::EVENT_TYPE_WINDOW_RESIZE;
        windowResizeEvent.eventInfo = windowResizeEventInfo;

        // Add the window move event to the queue
        wfe::editor::AddEvent(windowResizeEvent);

        break;
    }
    case ClientMessage:
        if(event.xclient.data.l[0] == editorPlatformInfo.deleteMessageAtom) {
            // Exit the application
            wfe::editor::CloseApplication(0);
        }
        break;
    }
}

// Public functions
void wfe::editor::CreatePlatform() {
    ConnectToX();
    CreateWindow();
}
void wfe::editor::DeletePlatform() {
    // Nothing required to delete the platform
}
void wfe::editor::PollPlatformEvents() {
    // Process every pending event from X
    XEvent xEvent;
    while(XPending(editorPlatformInfo.display)) {
        // Get the next event
        XNextEvent(editorPlatformInfo.display, &xEvent);

        // Process the event
        ProcessEvent(xEvent);
    }

    // Get every window rename event
    auto renameEvents = wfe::editor::GetEventsOfType(wfe::editor::EVENT_TYPE_WINDOW_RENAME);

    // Chech if any window rename events exist
    if(renameEvents.size()) {
        // Set the window's name to the last rename event
        wfe::editor::WindowRenameEventInfo* windowRenameEventInfo = (wfe::editor::WindowRenameEventInfo*)renameEvents.back().eventInfo;

        XStoreName(editorPlatformInfo.display, editorPlatformInfo.window, windowRenameEventInfo->newName.c_str());
    }
}

wfe::editor::PlatformInfo* wfe::editor::GetPlatformInfo() {
    return &editorPlatformInfo;
}

wfe::bool8_t wfe::editor::LocationExists(const wfe::string& location) {
    // Use stat to check if the file location exists
    struct stat info;
    return !stat(location.c_str(), &info);
}
void wfe::editor::CopyFiles(const wfe::string& srcFile, const wfe::string& dstFile, wfe::bool8_t folders) {
    // Modify the source path
    string srcFileMod = srcFile;
    if(folders)
        srcFileMod.push_back('.');
    
    // Create the command
    string fullCommand = (string)"cp -a " + srcFileMod + " " + dstFile;

    FILE* stream = popen(fullCommand.c_str(), "r");
    pclose(stream);
}
wfe::string wfe::editor::OpenFileDialog(const wfe::string& name, wfe::bool8_t& canceled, const wfe::string& startingLocation, wfe::bool8_t folders) {
    // Create the command
    string fullCommand = (string)"zenity --text=\'" + name + "\' --file-selection --filename=" + startingLocation;
    if(folders)
        fullCommand += (string)" --directory";
    FILE* stream = popen(fullCommand.c_str(), "r");

    // Read the folder location
    char_t buffer[512];
    char_t* result = fgets(buffer, 512, stream);

    // Get if the file dialog was canceled
    if(WEXITSTATUS(pclose(stream))) {
        canceled = true;
        return "";
    }

    // Return the final location
    string location = buffer;
    location[location.length() - 1] = 0;

    if(folders && location[location.length() - 1] != '/')
        location.push_back('/');

    return location;
}

VkResult wfe::editor::CreatePlatformSurface(VkInstance instance, const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface) {
    // Create the Xlib surface create info
    VkXlibSurfaceCreateInfoKHR createInfo;

    createInfo.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.dpy = editorPlatformInfo.display;
    createInfo.window = editorPlatformInfo.window;

    // Create the surface
    return vkCreateXlibSurfaceKHR(instance, &createInfo, pAllocator, pSurface);
}

#endif