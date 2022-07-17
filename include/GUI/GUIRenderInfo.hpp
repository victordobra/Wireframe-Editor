#pragma once

#include "Core.hpp"

namespace wfe::editor {
    struct GUIVertex {
        float32_t position[2];
        float32_t uvCoord[2];
        float32_t color[4];

        bool8_t operator==(const GUIVertex& other) {
            return position[0] == other.position[0] &&
                   position[1] == other.position[1] &&
                   uvCoord[0] == other.uvCoord[0] &&
                   uvCoord[1] == other.uvCoord[1] &&
                   color[0] == other.color[0] &&
                   color[1] == other.color[1] &&
                   color[2] == other.color[2] &&
                   color[3] == other.color[3];
        }
        bool8_t operator==(GUIVertex&& other) noexcept {
            return position[0] == other.position[0] &&
                   position[1] == other.position[1] &&
                   uvCoord[0] == other.uvCoord[0] &&
                   uvCoord[1] == other.uvCoord[1] &&
                   color[0] == other.color[0] &&
                   color[1] == other.color[1] &&
                   color[2] == other.color[2] &&
                   color[3] == other.color[3];
        }
    };

    struct GUIPushConstants {
        float32_t scale[2];
    };
}