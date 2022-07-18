#pragma once

#include "Core.hpp"
#include "GUI/GUIRenderInfo.hpp"
#include "Vulkan/VulkanInclude.hpp"

namespace wfe::editor {
    class Window {
    public:
        static const ptrdiff_t WINDOW_MARGIN = 3;
        static const ptrdiff_t WINDOW_MIN_SIZE = 150;

        struct WindowMesh {
            vector<GUIVertex> vertices;
            vector<uint32_t> indices;
        };

        size_t windowWidth = 200, windowHeight = 200;
        ptrdiff_t windowXPos = 100, windowYPos = 100;

        Window();
        Window(const Window&) = delete;
        Window(Window&&) noexcept = delete;

        Window& operator=(const Window&) = delete;
        Window& operator=(Window&&) noexcept = delete;

        void Bind(VkCommandBuffer commandBuffer);
        void Draw(VkCommandBuffer commandBuffer);

        static vector<Window*> GetWindows() {
            return windows;
        }

        virtual ~Window();
    private:
        WindowMesh GetWindowMesh();

        static vector<Window*> windows;

        ptrdiff_t cursorPrevX = 0, cursorPrevY = 0;
        bool8_t windowDrag = false;
        bool8_t windowResizeN = false, windowResizeS = false, windowResizeW = false, windowResizeE = false;

        VkBuffer vertexBuffer = NULL;
        VkDeviceMemory vertexMemory = NULL;
        VkBuffer indexBuffer = NULL;
        VkDeviceMemory indexMemory = NULL;

        size_t vertexCount = 0, indexCount = 0;
    };
}