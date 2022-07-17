#include "GUI/Window.hpp"
#include "Config/EditorColors.hpp"
#include "Vulkan/Device.hpp"
#include "IO/Cursor.hpp"

namespace wfe::editor {
    // Static variables
    vector<Window*> Window::windows;

    // Public member functions
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

        // Load the mesh
        WindowMesh windowMesh = GetWindowMesh();

        vertexCount = windowMesh.vertices.size();
        indexCount = windowMesh.indices.size();

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

        memcpy(vertexMappedMemory, windowMesh.vertices.data(), (size_t)vertexBufferSize);
        memcpy(indexMappedMemory, windowMesh.indices.data(), (size_t)indexBufferSize);

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

    // Private member functions
    Window::WindowMesh Window::GetWindowMesh() {
        CursorPos cursorPos = GetCursorPos();

        ptrdiff_t movementX = cursorPos.x - cursorPrevX;
        ptrdiff_t movementY = cursorPos.y - cursorPrevY;

        if(windowDragged) {
            // Move the window
            windowXPos += movementX;
            windowYPos += movementY;
        }
        // Resize the window based on the resize types
        if(CursorPressed()) {
            if(windowResizeN) {
                windowYPos += movementY;
                windowHeight -= movementY;
            }
            if(windowResizeS) {
                windowHeight += movementY;
            }

            if(windowResizeW) {
                windowXPos += movementX;
                windowWidth -= movementX;
            }
            if(windowResizeE) {
                windowWidth += movementX;
            }
        }
        
        // Set the cursor type based on resize modes
        if((windowResizeN && windowResizeE) || (windowResizeS && windowResizeW))
            SetCursorType(CURSOR_TYPE_SIZE_DIAGONAL_RIGHT);
        else if((windowResizeN && windowResizeW) || (windowResizeS && windowResizeE))
            SetCursorType(CURSOR_TYPE_SIZE_DIAGONAL_LEFT);
        else if(windowResizeN || windowResizeS)
            SetCursorType(CURSOR_TYPE_SIZE_UPDOWN);
        else if(windowResizeW || windowResizeE)
            SetCursorType(CURSOR_TYPE_SIZE_LEFTRIGHT);
        else 
            SetCursorType(CURSOR_TYPE_DEFAULT);

        // Check if the window should be dragged
        windowDragged = CursorPressed() && 
                        cursorPos.x >= windowXPos && 
                        cursorPos.x <= windowXPos + 98 && 
                        cursorPos.y >= windowYPos && 
                        cursorPos.y <= windowYPos + 18;

        // Check the window resize flags for every dimension
        windowResizeN = cursorPos.x >= windowXPos && 
                        cursorPos.x <= windowXPos + windowWidth && 
                        cursorPos.y >= windowYPos + 20 && 
                        cursorPos.y <= windowYPos + 20 + RESIZE_MARGIN;
        windowResizeS = cursorPos.x >= windowXPos && 
                        cursorPos.x <= windowXPos + windowWidth && 
                        cursorPos.y >= windowYPos + windowHeight - RESIZE_MARGIN && 
                        cursorPos.y <= windowYPos + windowHeight;
        windowResizeW = cursorPos.x >= windowXPos && 
                        cursorPos.x <= windowXPos + RESIZE_MARGIN && 
                        cursorPos.y >= windowYPos && 
                        cursorPos.y <= windowYPos + windowHeight;
        windowResizeE = cursorPos.x >= windowXPos + windowWidth - RESIZE_MARGIN && 
                        cursorPos.x <= windowXPos + windowWidth && 
                        cursorPos.y >= windowYPos && 
                        cursorPos.y <= windowYPos + windowHeight;
        
        // Set the new cursor coords
        cursorPrevX = cursorPos.x;
        cursorPrevY = cursorPos.y;

        WindowMesh mesh;

        // Extract the rgb components from the background and foreground color
        uint32_t backgroundColor = GetBackgroundColor();
        float32_t redBgd = ((backgroundColor >> 16) & 0xff) / 255.f;
        float32_t greenBgd = ((backgroundColor >> 0) & 0xff) / 255.f;
        float32_t blueBgd = (backgroundColor & 0xff) / 255.f;

        uint32_t foregroundColor = GetForegroundColor();
        float32_t redFgd = ((foregroundColor >> 16) & 0xff) / 255.f;
        float32_t greenFgd = ((foregroundColor >> 0) & 0xff) / 255.f;
        float32_t blueFgd = (foregroundColor & 0xff) / 255.f;

        // Create the top bar of the window; placeholder width at 48
        mesh.vertices.push_back({ { (float32_t)windowXPos,      (float32_t)windowYPos      }, { 0.f, 0.f }, { redFgd, greenFgd, blueFgd, 1.f } });
        mesh.vertices.push_back({ { (float32_t)windowXPos + 98, (float32_t)windowYPos      }, { 0.f, 0.f }, { redFgd, greenFgd, blueFgd, 1.f } });
        mesh.vertices.push_back({ { (float32_t)windowXPos + 98, (float32_t)windowYPos + 18 }, { 0.f, 0.f }, { redFgd, greenFgd, blueFgd, 1.f } });
        mesh.vertices.push_back({ { (float32_t)windowXPos,      (float32_t)windowYPos + 18 }, { 0.f, 0.f }, { redFgd, greenFgd, blueFgd, 1.f } });

        mesh.indices.push_back(0); mesh.indices.push_back(1); mesh.indices.push_back(3);
        mesh.indices.push_back(1); mesh.indices.push_back(2); mesh.indices.push_back(3);

        // Create the rest of the top bar
        mesh.vertices.push_back({ { (float32_t)windowXPos + 98,          (float32_t)windowYPos      }, { 0.f, 0.f }, { redBgd, greenBgd, blueBgd, 1.f } });
        mesh.vertices.push_back({ { (float32_t)windowXPos + windowWidth, (float32_t)windowYPos      }, { 0.f, 0.f }, { redBgd, greenBgd, blueBgd, 1.f } });
        mesh.vertices.push_back({ { (float32_t)windowXPos + windowWidth, (float32_t)windowYPos + 18 }, { 0.f, 0.f }, { redBgd, greenBgd, blueBgd, 1.f } });
        mesh.vertices.push_back({ { (float32_t)windowXPos + 98,          (float32_t)windowYPos + 18 }, { 0.f, 0.f }, { redBgd, greenBgd, blueBgd, 1.f } });

        mesh.indices.push_back(4); mesh.indices.push_back(5); mesh.indices.push_back(7);
        mesh.indices.push_back(5); mesh.indices.push_back(6); mesh.indices.push_back(7);

        // Create the main window body
        mesh.vertices.push_back({ { (float32_t)windowXPos,               (float32_t)windowYPos + 20           }, { 0.f, 0.f }, { redFgd, greenFgd, blueFgd, 1.f } });
        mesh.vertices.push_back({ { (float32_t)windowXPos + windowWidth, (float32_t)windowYPos + 20           }, { 0.f, 0.f }, { redFgd, greenFgd, blueFgd, 1.f } });
        mesh.vertices.push_back({ { (float32_t)windowXPos + windowWidth, (float32_t)windowYPos + windowHeight }, { 0.f, 0.f }, { redFgd, greenFgd, blueFgd, 1.f } });
        mesh.vertices.push_back({ { (float32_t)windowXPos,               (float32_t)windowYPos + windowHeight }, { 0.f, 0.f }, { redFgd, greenFgd, blueFgd, 1.f } });

        mesh.indices.push_back(8); mesh.indices.push_back(9);  mesh.indices.push_back(11);
        mesh.indices.push_back(9); mesh.indices.push_back(10); mesh.indices.push_back(11);

        return mesh;
    }
}