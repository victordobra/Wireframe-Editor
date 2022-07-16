#pragma once

#include "Core.hpp"
#include "GUI/GUIRenderInfo.hpp"
#include "Vulkan/VulkanInclude.hpp"

namespace wfe::editor {
    class Window {
    public:
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
        static vector<Window*> windows;

        size_t windowWidth = 100, windowHeight = 100, windowXPos = 0, windowYPos = 0;

        VkBuffer vertexBuffer = NULL;
        VkDeviceMemory vertexMemory = NULL;
        VkBuffer indexBuffer = NULL;
        VkDeviceMemory indexMemory = NULL;

        size_t vertexCount = 0, indexCount = 0;
    };
}