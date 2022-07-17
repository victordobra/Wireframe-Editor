#include "GUI/GUIRenderInfo.hpp"

namespace wfe {
    template<>
    void GetHashCode(editor::GUIVertex vertex, size_t& hash) {
        GetHashCode(vertex.position[0], hash);
        CombineHash(hash, vertex.position[1]);
        CombineHash(hash, vertex.uvCoord[0]);
        CombineHash(hash, vertex.uvCoord[1]);
        CombineHash(hash, vertex.color[0]);
        CombineHash(hash, vertex.color[1]);
        CombineHash(hash, vertex.color[2]);
        CombineHash(hash, vertex.color[3]);
    }
}