#pragma once

#include "Core.hpp"

namespace wfe::editor {
    struct GUIVertex {
        float32_t position[2];
        float32_t uvCoord[2];
        float32_t color[4];
    };

    struct GUIPushConstants {
        float32_t scale[2];
    };
}