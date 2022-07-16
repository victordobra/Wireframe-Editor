#include "GUI/Window.hpp"
#include "Config/EditorColors.hpp"
#include "Vulkan/Device.hpp"

namespace wfe::editor {
    // Static variables
    vector<Window*> Window::windows;

    // Member functions
    Window::Window() {
        windows.push_back(this);
    }

    void Window::Bind(VkCommandBuffer commandBuffer) {
        // Delete the vertex and index buffers from the previous frame
        if(vertexBuffer) {
            vkDeviceWaitIdle(GetDevice());
            vkDestroyBuffer(GetDevice(), vertexBuffer, nullptr);
        }
        if(vertexMemory) {
            vkDeviceWaitIdle(GetDevice());
            vkFreeMemory(GetDevice(), vertexMemory, nullptr);
        }
        if(indexBuffer) {
            vkDeviceWaitIdle(GetDevice());
            vkDestroyBuffer(GetDevice(), indexBuffer, nullptr);
        }
        if(indexMemory) {
            vkDeviceWaitIdle(GetDevice());
            vkFreeMemory(GetDevice(), indexMemory, nullptr);
        }

        // Create placeholder vertices and indices
        float32_t red = ((GetForegroundColor() >> 16) & 0xff) / 255.f;
        float32_t green = ((GetForegroundColor() >> 8) & 0xff) / 255.f;
        float32_t blue = (GetForegroundColor() & 0xff) / 255.f;

        GUIVertex vertices[4]{
            { { 250.f, 250.f }, { 0.f, 0.f }, { red, green, blue, 1.f } },
            { { 500.f, 250.f }, { 0.f, 0.f }, { red, green, blue, 1.f } },
            { { 500.f, 500.f }, { 0.f, 0.f }, { red, green, blue, 1.f } },
            { { 250.f, 500.f }, { 0.f, 0.f }, { red, green, blue, 1.f } }
        };
        uint32_t indices[6]{ 0, 1, 3, 1, 2, 3 };

        vertexCount = 4;
        indexCount = 6;

        // Create the vertex and index buffer
        VkDeviceSize vertexBufferSize = sizeof(GUIVertex) * vertexCount;
        VkDeviceSize indexBufferSize = sizeof(uint32_t) * indexCount;

        vertexBufferSize = (vertexBufferSize + GetDeviceProperties().limits.nonCoherentAtomSize - 1) & ~(GetDeviceProperties().limits.nonCoherentAtomSize - 1);
        indexBufferSize = (indexBufferSize + GetDeviceProperties().limits.nonCoherentAtomSize - 1) & ~(GetDeviceProperties().limits.nonCoherentAtomSize - 1);

        CreateBuffer(vertexBufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, vertexBuffer, vertexMemory);
        CreateBuffer(indexBufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, indexBuffer, indexMemory);

        // Write the the vertex and index buffers
        void* vertexMappedMemory;
        void* indexMappedMemory;

        vkMapMemory(GetDevice(), vertexMemory, 0, vertexBufferSize, 0, &vertexMappedMemory);
        vkMapMemory(GetDevice(), indexMemory, 0, indexBufferSize, 0, &indexMappedMemory);

        memcpy(vertexMappedMemory, vertices, (size_t)vertexBufferSize);
        memcpy(indexMappedMemory, indices, (size_t)indexBufferSize);

        VkMappedMemoryRange mappedMemoryRanges[2];

        mappedMemoryRanges[0].sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        mappedMemoryRanges[0].pNext = nullptr;
        mappedMemoryRanges[0].memory = vertexMemory;
        mappedMemoryRanges[0].offset = 0;
        mappedMemoryRanges[0].size = vertexBufferSize;

        mappedMemoryRanges[1].sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        mappedMemoryRanges[1].pNext = nullptr;
        mappedMemoryRanges[1].memory = indexMemory;
        mappedMemoryRanges[1].offset = 0;
        mappedMemoryRanges[1].size = indexBufferSize;

        vkFlushMappedMemoryRanges(GetDevice(), 2, mappedMemoryRanges);

        vkUnmapMemory(GetDevice(), vertexMemory);
        vkUnmapMemory(GetDevice(), indexMemory);

        // Bind the vertex and index buffers
        VkDeviceSize offset = 0;
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertexBuffer, &offset);
        vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);
    }
    void Window::Draw(VkCommandBuffer commandBuffer) {
        vkCmdDrawIndexed(commandBuffer, (uint32_t)indexCount, 1, 0, 0, 0);
    }

    Window::~Window() {
        // Delete the vertex and index buffers from the previous frame
        if(vertexBuffer) {
            vkDeviceWaitIdle(GetDevice());
            vkDestroyBuffer(GetDevice(), vertexBuffer, nullptr);
        }
        if(vertexMemory) {
            vkDeviceWaitIdle(GetDevice());
            vkFreeMemory(GetDevice(), vertexMemory, nullptr);
        }
        if(indexBuffer) {
            vkDeviceWaitIdle(GetDevice());
            vkDestroyBuffer(GetDevice(), indexBuffer, nullptr);
        }
        if(indexMemory) {
            vkDeviceWaitIdle(GetDevice());
            vkFreeMemory(GetDevice(), indexMemory, nullptr);
        }

        // Remove itself from the window list
        for(auto*& window : windows)
            if(window == this) {
                windows.erase(&window);
                break;
            }
    }
}