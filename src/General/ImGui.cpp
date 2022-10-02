#include "General/ImGui.hpp"
#include "General/Application.hpp"

namespace wfe::editor {
    // Constants
    const float32_t FONT_SIZE = 15.f;

    // Variables
    ImGuiContext* context;
    ImGuiIO* io;
    ImGuiStyle* style;
    ImFont* normalFont,* boldFont,* italicFont;

    // Public functions
    void CreateImGui() {
        // Check the ImGui version
        IMGUI_CHECKVERSION();

        // Create the ImGui context
        context = ImGui::CreateContext();

        // Set the ImGui IO info
        io = &ImGui::GetIO();

        io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

        io->LogFilename = "imgui-log.txt";
        io->IniFilename = nullptr;
        
        // TODO: Implement clipboard callbacks
        io->GetClipboardTextFn = nullptr;
        io->SetClipboardTextFn = nullptr;

        io->DisplaySize = { (float32_t)GetMainWindowWidth(), (float32_t)GetMainWindowHeight() };

        // Load the fonts
#ifdef NDEBUG
        normalFont = io->Fonts->AddFontFromFileTTF("assets/fonts/Consola-Default.ttf", FONT_SIZE);
        boldFont   = io->Fonts->AddFontFromFileTTF("assets/fonts/Consola-Bold.ttf", FONT_SIZE);
        italicFont = io->Fonts->AddFontFromFileTTF("assets/fonts/Consola-Italic.ttf", FONT_SIZE);
#else
        normalFont = io->Fonts->AddFontFromFileTTF("..assets/fonts/Consola-Default.ttf", FONT_SIZE);
        boldFont   = io->Fonts->AddFontFromFileTTF("..assets/fonts/Consola-Bold.ttf", FONT_SIZE);
        italicFont = io->Fonts->AddFontFromFileTTF("..assets/fonts/Consola-Italic.ttf", FONT_SIZE);
#endif
        
        io->FontDefault = normalFont;

        // Set the style
        ImGui::StyleColorsDark();

        style = &ImGui::GetStyle();

        style->Colors[ImGuiCol_WindowBg]           = ImVec4{ .1000f, .1050f, .1100f, 1.f };

        // Headers
        style->Colors[ImGuiCol_Header]             = ImVec4{ .2000f, .2050f, .2100f, 1.f };
        style->Colors[ImGuiCol_HeaderHovered]      = ImVec4{ .3000f, .3050f, .3100f, 1.f };
        style->Colors[ImGuiCol_HeaderActive]       = ImVec4{ .1500f, .1505f, .1510f, 1.f };

        // Buttons
        style->Colors[ImGuiCol_Button]             = ImVec4{ .2500f, .2550f, .2600f, 1.f };
        style->Colors[ImGuiCol_ButtonHovered]      = ImVec4{ .3500f, .3550f, .3600f, 1.f };
        style->Colors[ImGuiCol_ButtonActive]       = ImVec4{ .2000f, .2005f, .2010f, 1.f };

        // Frame BG
        style->Colors[ImGuiCol_FrameBg]            = ImVec4{ .2000f, .2050f, .2100f, 1.f };
        style->Colors[ImGuiCol_FrameBgHovered]     = ImVec4{ .3000f, .3050f, .3100f, 1.f };
        style->Colors[ImGuiCol_FrameBgActive]      = ImVec4{ .1500f, .1505f, .1510f, 1.f };

        // Tabs
        style->Colors[ImGuiCol_Tab]                = ImVec4{ .1500f, .1505f, .1510f, 1.f };
        style->Colors[ImGuiCol_TabHovered]         = ImVec4{ .3800f, .3805f, .3810f, 1.f };
        style->Colors[ImGuiCol_TabActive]          = ImVec4{ .2800f, .2805f, .2810f, 1.f };
        style->Colors[ImGuiCol_TabUnfocused]       = ImVec4{ .1500f, .1505f, .1510f, 1.f };
        style->Colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ .2000f, .2050f, .2100f, 1.f };

        // Title
        style->Colors[ImGuiCol_TitleBg]            = ImVec4{ .1500f, .1505f, .1510f, 1.f };
        style->Colors[ImGuiCol_TitleBgActive]      = ImVec4{ .1700f, .1705f, .1710f, 1.f };
        style->Colors[ImGuiCol_TitleBgCollapsed]   = ImVec4{ .1500f, .1505f, .1510f, 1.f };
    }
    void DeleteImGui() {
        ImGui::DestroyContext(context);
    }
    void ProcessImGuiEvents() {
        // Process every key event
        auto keyEvents = GetEventsOfType(EVENT_TYPE_KEY);

        for(auto& keyEvent : keyEvents) {
            KeyEventInfo* keyEventInfo = (KeyEventInfo*)keyEvent.eventInfo;

            io->AddKeyEvent(keyEventInfo->keyCode, keyEventInfo->pressed);
        }

        // Process every input char event
        auto inputCharEvents = GetEventsOfType(EVENT_TYPE_INPUT_CHAR);

        for(auto& inputCharEvent : inputCharEvents) {
            InputCharEventInfo* inputCharEventInfo = (InputCharEventInfo*)inputCharEvent.eventInfo;

            io->AddInputCharactersUTF8(inputCharEventInfo->inputString.c_str());
        }

        // Process every mosue button event
        auto mouseButtonEvents = GetEventsOfType(EVENT_TYPE_MOUSE_BUTTON);

        for(auto& mouseButtonEvent : mouseButtonEvents) {
            MouseButtonEventInfo* mouseButtonEventInfo = (MouseButtonEventInfo*)mouseButtonEvent.eventInfo;

            io->AddMouseButtonEvent(mouseButtonEventInfo->mouseButton, mouseButtonEventInfo->pressed);
        }

        // Process every mouse move event
        auto mouseMoveEvents = GetEventsOfType(EVENT_TYPE_MOUSE_MOVE);

        for(auto& mouseMoveEvent : mouseMoveEvents) {
            MouseMoveEventInfo* mouseMoveEventInfo = (MouseMoveEventInfo*)mouseMoveEvent.eventInfo;

            if(mouseMoveEventInfo->mouseX != -1 && mouseMoveEventInfo->mouseY != -1)
                io->AddMousePosEvent(mouseMoveEventInfo->mouseX, mouseMoveEventInfo->mouseY);
            io->AddMouseWheelEvent(0.f, mouseMoveEventInfo->scrollWheelDelta);
        }

        // Process the last window resize event (if it exists)
        auto windowResizeEvents = GetEventsOfType(EVENT_TYPE_WINDOW_RESIZE);

        if(windowResizeEvents.size()) {
            WindowResizeEventInfo* windowResizeEventInfo = (WindowResizeEventInfo*)windowResizeEvents.back().eventInfo;

            io->DisplaySize = { (float32_t)windowResizeEventInfo->windowWidth, (float32_t)windowResizeEventInfo->windowHeight };
        }
    }

    ImGuiContext* GetImGuiContext() {
        return context;
    }
    ImGuiIO* GetImGuiIO() {
        return io;
    }
    ImGuiStyle* GetImGuiStyle() {
        return style;
    }

    ImFont* GetImGuiNormalFont() {
        return normalFont;
    }
    ImFont* GetImGuiBoldFont() {
        return boldFont;
    }
    ImFont* GetImGuiItalicFont() {
        return italicFont;
    }
}