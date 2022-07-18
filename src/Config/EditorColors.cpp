#include "Config/EditorColors.hpp"

namespace wfe::editor {
    // Colors
    uint32_t backgroundColor = 0x222222;
    uint32_t edgeColor = 0x333333;
    uint32_t foregroundColor = 0x444444;
    uint32_t highlightColor = 0x999999;
    uint32_t textColor = 0xeeeeff;
    uint32_t dockingColor = 0x000077;

    // Functions
    void LoadColors() {
        FileInput input("config/colors.wfcolors", STREAM_TYPE_BINARY);

        if(!input)
            return;
        
        input.ReadBuffer((char_t*)&backgroundColor, sizeof(uint32_t));
        input.ReadBuffer((char_t*)&edgeColor, sizeof(uint32_t));
        input.ReadBuffer((char_t*)&foregroundColor, sizeof(uint32_t));
        input.ReadBuffer((char_t*)&highlightColor, sizeof(uint32_t));
        input.ReadBuffer((char_t*)&textColor, sizeof(uint32_t));
        input.ReadBuffer((char_t*)&dockingColor, sizeof(uint32_t));

        input.Close();
    }
    void SaveColors() {
        FileOutput output("config/colors.wfcolors", STREAM_TYPE_BINARY);

        output.WriteBuffer((char_t*)&backgroundColor, sizeof(uint32_t));
        output.WriteBuffer((char_t*)&edgeColor, sizeof(uint32_t));
        output.WriteBuffer((char_t*)&foregroundColor, sizeof(uint32_t));
        output.WriteBuffer((char_t*)&highlightColor, sizeof(uint32_t));
        output.WriteBuffer((char_t*)&textColor, sizeof(uint32_t));
        output.WriteBuffer((char_t*)&dockingColor, sizeof(uint32_t));

        output.Close();
    }

    uint32_t GetBackgroundColor() {
        return backgroundColor;
    }
    uint32_t GetEdgeColor() {
        return edgeColor;
    }
    uint32_t GetForegroundColor() {
        return foregroundColor;
    }
    uint32_t GetHighlightColor() {
        return highlightColor;
    }
    uint32_t GetTextColor() {
        return textColor;
    }
    uint32_t GetDockingColor() {
        return dockingColor;
    }

    void SetBackgroundColor(uint32_t newColor) {
        backgroundColor = newColor;
    }
    void SetEdgeColor(uint32_t newColor) {
        edgeColor = newColor;
    } 
    void SetForegroundColor(uint32_t newColor) {
        foregroundColor = newColor;
    }
    void SetHighlightColor(uint32_t newColor) {
        highlightColor = newColor;
    }
    void SetTextColor(uint32_t newColor) {
        textColor = newColor;
    }
    void SetDockingColor(uint32_t newColor) {
        dockingColor = newColor;
    }
}