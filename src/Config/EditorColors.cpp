#include "Config/EditorColors.hpp"

namespace wfe::editor {
    // Colors
    uint32_t backgroundColor;
    uint32_t foregroundColor;
    uint32_t highlightColor;
    uint32_t textColor;

    // Functions
    void LoadColors() {
        FileInput input("config/colors.wfcolors", STREAM_TYPE_BINARY);

        FILE* file = *(FILE**)&input;
        
        input.ReadBuffer((char_t*)&backgroundColor, sizeof(uint32_t));
        input.ReadBuffer((char_t*)&foregroundColor, sizeof(uint32_t));
        input.ReadBuffer((char_t*)&highlightColor, sizeof(uint32_t));
        input.ReadBuffer((char_t*)&textColor, sizeof(uint32_t));

        input.Close();
    }
    void SaveColors() {
        FileOutput output("config/colors.wfcolors", STREAM_TYPE_BINARY);

        output.WriteBuffer((char_t*)&backgroundColor, sizeof(uint32_t));
        output.WriteBuffer((char_t*)&foregroundColor, sizeof(uint32_t));
        output.WriteBuffer((char_t*)&highlightColor, sizeof(uint32_t));
        output.WriteBuffer((char_t*)&textColor, sizeof(uint32_t));

        output.Close();
    }

    uint32_t GetBackgroundColor() {
        return backgroundColor;
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

    void SetBackgroundColor(uint32_t newColor) {
        backgroundColor = newColor;
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
}