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

        if(windowDrag) {
            // Move the window
            windowXPos += movementX;
            windowYPos += movementY;
        }
        // Resize the window based on the resize types
        if(windowResizeN) {
            windowYPos += movementY;
            windowHeight -= movementY;
            windowHeight = windowHeight < WINDOW_MIN_SIZE ? WINDOW_MIN_SIZE : windowHeight;
        }
        if(windowResizeS) {
            windowHeight += movementY;
            windowHeight = windowHeight < WINDOW_MIN_SIZE ? WINDOW_MIN_SIZE : windowHeight;
        }

        if(windowResizeW) {
            windowXPos += movementX;
            windowWidth -= movementX;
            windowWidth = windowWidth < WINDOW_MIN_SIZE ? WINDOW_MIN_SIZE : windowWidth;
        }
        if(windowResizeE) {
            windowWidth += movementX;
            windowWidth = windowWidth < WINDOW_MIN_SIZE ? WINDOW_MIN_SIZE : windowWidth;
        }
        
        // Check the window resize flags for every dimension
        bool8_t windowCanResizeN = cursorPos.x >= windowXPos && 
                                   cursorPos.x <= windowXPos + windowWidth && 
                                   cursorPos.y >= windowYPos && 
                                   cursorPos.y <= windowYPos + 2 * WINDOW_MARGIN;
        bool8_t windowCanResizeS = cursorPos.x >= windowXPos && 
                                   cursorPos.x <= windowXPos + windowWidth && 
                                   cursorPos.y >= windowYPos + windowHeight - WINDOW_MARGIN && 
                                   cursorPos.y <= windowYPos + windowHeight + WINDOW_MARGIN;
        bool8_t windowCanResizeW = cursorPos.x >= windowXPos - WINDOW_MARGIN && 
                                   cursorPos.x <= windowXPos + WINDOW_MARGIN && 
                                   cursorPos.y >= windowYPos && 
                                   cursorPos.y <= windowYPos + windowHeight;
        bool8_t windowCanResizeE = cursorPos.x >= windowXPos + windowWidth - WINDOW_MARGIN && 
                                   cursorPos.x <= windowXPos + windowWidth + WINDOW_MARGIN && 
                                   cursorPos.y >= windowYPos && 
                                   cursorPos.y <= windowYPos + windowHeight;

        // Set the cursor type based on resize modes
        if(((windowCanResizeN && windowCanResizeE) || (windowCanResizeS && windowCanResizeW)) || ((windowResizeN && windowResizeE) || (windowResizeS && windowResizeW)))
            SetCursorType(CURSOR_TYPE_SIZE_DIAGONAL_RIGHT);
        else if(((windowCanResizeN && windowCanResizeW) || (windowCanResizeS && windowCanResizeE)) || ((windowResizeN && windowResizeW) || (windowResizeS && windowResizeE)))
            SetCursorType(CURSOR_TYPE_SIZE_DIAGONAL_LEFT);
        else if((windowCanResizeN || windowCanResizeS) || (windowResizeN || windowResizeS))
            SetCursorType(CURSOR_TYPE_SIZE_UPDOWN);
        else if((windowCanResizeW || windowCanResizeE) || (windowResizeW || windowResizeE))
            SetCursorType(CURSOR_TYPE_SIZE_LEFTRIGHT); 

        // Check if the window should be dragged
        if(CursorPressed() && 
           cursorPos.x >  windowXPos + WINDOW_MARGIN && 
           cursorPos.x <  windowXPos + WINDOW_MARGIN + 100 && 
           cursorPos.y >  windowYPos + 2 * WINDOW_MARGIN && 
           cursorPos.y <  windowYPos + WINDOW_MARGIN + WINDOW_TOP_BAR_HEIGHT)
            windowDrag = true;
        
        if(CursorPressed() && windowCanResizeN)
            windowResizeN = true;
        if(CursorPressed() && windowCanResizeS)
            windowResizeS = true;
        if(CursorPressed() && windowCanResizeW)
            windowResizeW = true;
        if(CursorPressed() && windowCanResizeE)
            windowResizeE = true;

        // Reset every drag/resize state if the cursor is released
        if(!CursorDown()) {
            windowDrag = false;
            windowResizeN = false;
            windowResizeS = false;
            windowResizeW = false;
            windowResizeE = false;
        }
        
        // Set the new cursor coords
        cursorPrevX = cursorPos.x;
        cursorPrevY = cursorPos.y;

        if(windowDrag) {
            // Move the window to the end of the windows vector
            for(auto*& window : windows)
                if(window == this) {
                    windows.erase(&window);
                    break;
                }
            windows.insert(windows.begin(), this);
        }

        WindowMesh mesh;

        // Extract the rgb components from the background and foreground color
        uint32_t edgeColor = GetEdgeColor();
        float32_t redE = ((edgeColor >> 16) & 0xff) / 255.f;
        float32_t greenE = ((edgeColor >> 8) & 0xff) / 255.f;
        float32_t blueE = (edgeColor & 0xff) / 255.f;

        uint32_t foregroundColor = GetForegroundColor();
        float32_t redFgd = ((foregroundColor >> 16) & 0xff) / 255.f;
        float32_t greenFgd = ((foregroundColor >> 8) & 0xff) / 255.f;
        float32_t blueFgd = (foregroundColor & 0xff) / 255.f;

        uint32_t dockingColor = GetDockingColor();
        float32_t redDock = ((dockingColor >> 16) & 0xff) / 255.f;
        float32_t greenDock = ((dockingColor >> 8) & 0xff) / 255.f;
        float32_t blueDock = (dockingColor & 0xff) / 255.f;

        uint32_t dockPreviewColor = GetDockPreviewColor();
        float32_t redDockPrev = ((dockPreviewColor >> 16) & 0xff) / 255.f;
        float32_t greenDockPrev = ((dockPreviewColor >> 8) & 0xff) / 255.f;
        float32_t blueDockPrev = (dockPreviewColor & 0xff) / 255.f;

        // Create the top bar of the window; placeholder width at 100
        mesh.vertices.push_back({ { (float32_t)windowXPos + WINDOW_MARGIN,       (float32_t)windowYPos + WINDOW_MARGIN      }, { 0.f, 0.f }, { redFgd, greenFgd, blueFgd, 1.f } });
        mesh.vertices.push_back({ { (float32_t)windowXPos + WINDOW_MARGIN + 100, (float32_t)windowYPos + WINDOW_MARGIN      }, { 0.f, 0.f }, { redFgd, greenFgd, blueFgd, 1.f } });
        mesh.vertices.push_back({ { (float32_t)windowXPos + WINDOW_MARGIN + 100, (float32_t)windowYPos + WINDOW_MARGIN + 20 }, { 0.f, 0.f }, { redFgd, greenFgd, blueFgd, 1.f } });
        mesh.vertices.push_back({ { (float32_t)windowXPos + WINDOW_MARGIN,       (float32_t)windowYPos + WINDOW_MARGIN + 20 }, { 0.f, 0.f }, { redFgd, greenFgd, blueFgd, 1.f } });

        mesh.indices.push_back(0); mesh.indices.push_back(1); mesh.indices.push_back(3);
        mesh.indices.push_back(1); mesh.indices.push_back(2); mesh.indices.push_back(3);

        // Create the main window body
        mesh.vertices.push_back({ { (float32_t)windowXPos + WINDOW_MARGIN,               (float32_t)windowYPos + 2 * WINDOW_MARGIN + WINDOW_TOP_BAR_HEIGHT }, { 0.f, 0.f }, { redFgd, greenFgd, blueFgd, 1.f } });
        mesh.vertices.push_back({ { (float32_t)windowXPos + windowWidth - WINDOW_MARGIN, (float32_t)windowYPos + 2 * WINDOW_MARGIN + WINDOW_TOP_BAR_HEIGHT }, { 0.f, 0.f }, { redFgd, greenFgd, blueFgd, 1.f } });
        mesh.vertices.push_back({ { (float32_t)windowXPos + windowWidth - WINDOW_MARGIN, (float32_t)windowYPos + windowHeight - WINDOW_MARGIN              }, { 0.f, 0.f }, { redFgd, greenFgd, blueFgd, 1.f } });
        mesh.vertices.push_back({ { (float32_t)windowXPos + WINDOW_MARGIN,               (float32_t)windowYPos + windowHeight - WINDOW_MARGIN              }, { 0.f, 0.f }, { redFgd, greenFgd, blueFgd, 1.f } });

        mesh.indices.push_back(4); mesh.indices.push_back(5); mesh.indices.push_back(7);
        mesh.indices.push_back(5); mesh.indices.push_back(6); mesh.indices.push_back(7);

        // Create the window edge 
        mesh.vertices.push_back({ { (float32_t)windowXPos + WINDOW_MARGIN,           (float32_t)windowYPos + WINDOW_MARGIN                             }, { 0.f, 0.f }, { redE, greenE, blueE, 1.f } });
        mesh.vertices.push_back({ { (float32_t)windowXPos + WINDOW_MARGIN + 100,     (float32_t)windowYPos + WINDOW_MARGIN                             }, { 0.f, 0.f }, { redE, greenE, blueE, 1.f } });
        mesh.vertices.push_back({ { (float32_t)windowXPos + WINDOW_MARGIN + 100,     (float32_t)windowYPos + WINDOW_MARGIN + WINDOW_TOP_BAR_HEIGHT     }, { 0.f, 0.f }, { redE, greenE, blueE, 1.f } });
        mesh.vertices.push_back({ { (float32_t)windowXPos + WINDOW_MARGIN,           (float32_t)windowYPos + WINDOW_MARGIN + WINDOW_TOP_BAR_HEIGHT     }, { 0.f, 0.f }, { redE, greenE, blueE, 1.f } });
        mesh.vertices.push_back({ { (float32_t)windowXPos,                           (float32_t)windowYPos                                             }, { 0.f, 0.f }, { redE, greenE, blueE, 1.f } });
        mesh.vertices.push_back({ { (float32_t)windowXPos + 100 + 2 * WINDOW_MARGIN, (float32_t)windowYPos                                             }, { 0.f, 0.f }, { redE, greenE, blueE, 1.f } });
        mesh.vertices.push_back({ { (float32_t)windowXPos + 100 + 2 * WINDOW_MARGIN, (float32_t)windowYPos + 2 * WINDOW_MARGIN + WINDOW_TOP_BAR_HEIGHT }, { 0.f, 0.f }, { redE, greenE, blueE, 1.f } });
        mesh.vertices.push_back({ { (float32_t)windowXPos,                           (float32_t)windowYPos + 2 * WINDOW_MARGIN + WINDOW_TOP_BAR_HEIGHT }, { 0.f, 0.f }, { redE, greenE, blueE, 1.f } });

        mesh.indices.push_back(12); mesh.indices.push_back(8 ); mesh.indices.push_back(15);
        mesh.indices.push_back(8 ); mesh.indices.push_back(11); mesh.indices.push_back(15);
        mesh.indices.push_back(11); mesh.indices.push_back(14); mesh.indices.push_back(15);
        mesh.indices.push_back(11); mesh.indices.push_back(10); mesh.indices.push_back(14);
        mesh.indices.push_back(9 ); mesh.indices.push_back(14); mesh.indices.push_back(10);
        mesh.indices.push_back(9 ); mesh.indices.push_back(13); mesh.indices.push_back(14);
        mesh.indices.push_back(12); mesh.indices.push_back(9 ); mesh.indices.push_back(8 );
        mesh.indices.push_back(12); mesh.indices.push_back(13); mesh.indices.push_back(9 );

        mesh.vertices.push_back({ { (float32_t)windowXPos + WINDOW_MARGIN,               (float32_t)windowYPos + 2 * WINDOW_MARGIN + WINDOW_TOP_BAR_HEIGHT }, { 0.f, 0.f }, { redE, greenE, blueE, 1.f } });
        mesh.vertices.push_back({ { (float32_t)windowXPos + windowWidth - WINDOW_MARGIN, (float32_t)windowYPos + 2 * WINDOW_MARGIN + WINDOW_TOP_BAR_HEIGHT }, { 0.f, 0.f }, { redE, greenE, blueE, 1.f } });
        mesh.vertices.push_back({ { (float32_t)windowXPos + windowWidth - WINDOW_MARGIN, (float32_t)windowYPos + windowHeight - WINDOW_MARGIN              }, { 0.f, 0.f }, { redE, greenE, blueE, 1.f } });
        mesh.vertices.push_back({ { (float32_t)windowXPos + WINDOW_MARGIN,               (float32_t)windowYPos + windowHeight - WINDOW_MARGIN              }, { 0.f, 0.f }, { redE, greenE, blueE, 1.f } });
        mesh.vertices.push_back({ { (float32_t)windowXPos,                               (float32_t)windowYPos + WINDOW_MARGIN + WINDOW_TOP_BAR_HEIGHT     }, { 0.f, 0.f }, { redE, greenE, blueE, 1.f } });
        mesh.vertices.push_back({ { (float32_t)windowXPos + windowWidth,                 (float32_t)windowYPos + WINDOW_MARGIN + WINDOW_TOP_BAR_HEIGHT     }, { 0.f, 0.f }, { redE, greenE, blueE, 1.f } });
        mesh.vertices.push_back({ { (float32_t)windowXPos + windowWidth,                 (float32_t)windowYPos + windowHeight                              }, { 0.f, 0.f }, { redE, greenE, blueE, 1.f } });
        mesh.vertices.push_back({ { (float32_t)windowXPos,                               (float32_t)windowYPos + windowHeight                              }, { 0.f, 0.f }, { redE, greenE, blueE, 1.f } });

        mesh.indices.push_back(20); mesh.indices.push_back(16); mesh.indices.push_back(23);
        mesh.indices.push_back(16); mesh.indices.push_back(19); mesh.indices.push_back(23);
        mesh.indices.push_back(11); mesh.indices.push_back(22); mesh.indices.push_back(23);
        mesh.indices.push_back(11); mesh.indices.push_back(18); mesh.indices.push_back(22);
        mesh.indices.push_back(17); mesh.indices.push_back(22); mesh.indices.push_back(18);
        mesh.indices.push_back(17); mesh.indices.push_back(21); mesh.indices.push_back(22);
        mesh.indices.push_back(20); mesh.indices.push_back(17); mesh.indices.push_back(16);
        mesh.indices.push_back(20); mesh.indices.push_back(21); mesh.indices.push_back(17);

        size_t vertexCount = mesh.vertices.size();

        // Check every window to see if any docking highlights are needed
        if(windowDrag)
            for(auto* window : windows) {
                if(window == this)
                    continue;
                
                if(!(cursorPos.x >= window->windowXPos + WINDOW_MARGIN &&
                     cursorPos.x <= window->windowXPos + window->windowWidth - WINDOW_MARGIN &&
                     cursorPos.y >= window->windowYPos + 2 * WINDOW_MARGIN + WINDOW_TOP_BAR_HEIGHT &&
                     cursorPos.y <= window->windowYPos + window->windowHeight - WINDOW_MARGIN))
                    continue;
                
                ptrdiff_t windowXMid = (2 * window->windowXPos + window->windowWidth) >> 1;
                ptrdiff_t windowYMid = (2 * window->windowYPos + 2 * WINDOW_MARGIN + WINDOW_TOP_BAR_HEIGHT + window->windowHeight) >> 1;

                // Top preview
                if(cursorPos.x >= windowXMid - WINDOW_DOCK_HIGHLIGHT_SIZE &&
                   cursorPos.x <= windowXMid + WINDOW_DOCK_HIGHLIGHT_SIZE &&
                   cursorPos.y >= window->windowYPos + 2 * WINDOW_MARGIN + WINDOW_TOP_BAR_HEIGHT &&
                   cursorPos.y <= window->windowYPos + 2 * WINDOW_MARGIN + WINDOW_TOP_BAR_HEIGHT + WINDOW_DOCK_HIGHLIGHT_SIZE) {
                    size_t newWidth = window->windowWidth;
                    size_t newHeight = window->windowHeight >> 1;
                    newHeight = (newHeight < WINDOW_MIN_SIZE) ? WINDOW_MIN_SIZE : newHeight;

                    mesh.vertices.push_back({ { (float32_t)window->windowXPos,            (float32_t)window->windowYPos             }, { 0.f, 0.f }, { redDockPrev, greenDockPrev, blueDockPrev, 1.f } });
                    mesh.vertices.push_back({ { (float32_t)window->windowXPos + newWidth, (float32_t)window->windowYPos             }, { 0.f, 0.f }, { redDockPrev, greenDockPrev, blueDockPrev, 1.f } });
                    mesh.vertices.push_back({ { (float32_t)window->windowXPos + newWidth, (float32_t)window->windowYPos + newHeight }, { 0.f, 0.f }, { redDockPrev, greenDockPrev, blueDockPrev, 1.f } });
                    mesh.vertices.push_back({ { (float32_t)window->windowXPos,            (float32_t)window->windowYPos + newHeight }, { 0.f, 0.f }, { redDockPrev, greenDockPrev, blueDockPrev, 1.f } });

                    mesh.indices.push_back(vertexCount    ); mesh.indices.push_back(vertexCount + 1); mesh.indices.push_back(vertexCount + 3);
                    mesh.indices.push_back(vertexCount + 1); mesh.indices.push_back(vertexCount + 2); mesh.indices.push_back(vertexCount + 3);

                    vertexCount = mesh.vertices.size();

                    break;
                }

                // Bottom preview
                if(cursorPos.x >= windowXMid - WINDOW_DOCK_HIGHLIGHT_SIZE &&
                   cursorPos.x <= windowXMid + WINDOW_DOCK_HIGHLIGHT_SIZE &&
                   cursorPos.y >= window->windowYPos + window->windowHeight - WINDOW_MARGIN - WINDOW_DOCK_HIGHLIGHT_SIZE &&
                   cursorPos.y <= window->windowYPos + window->windowHeight - WINDOW_MARGIN) {
                    size_t newWidth = window->windowWidth;
                    size_t newHeight = window->windowHeight >> 1;
                    newHeight = (newHeight < WINDOW_MIN_SIZE) ? WINDOW_MIN_SIZE : newHeight;

                    mesh.vertices.push_back({ { (float32_t)window->windowXPos,            (float32_t)window->windowYPos + newHeight     }, { 0.f, 0.f }, { redDockPrev, greenDockPrev, blueDockPrev, 1.f } });
                    mesh.vertices.push_back({ { (float32_t)window->windowXPos + newWidth, (float32_t)window->windowYPos + newHeight     }, { 0.f, 0.f }, { redDockPrev, greenDockPrev, blueDockPrev, 1.f } });
                    mesh.vertices.push_back({ { (float32_t)window->windowXPos + newWidth, (float32_t)window->windowYPos + 2 * newHeight }, { 0.f, 0.f }, { redDockPrev, greenDockPrev, blueDockPrev, 1.f } });
                    mesh.vertices.push_back({ { (float32_t)window->windowXPos,            (float32_t)window->windowYPos + 2 * newHeight }, { 0.f, 0.f }, { redDockPrev, greenDockPrev, blueDockPrev, 1.f } });

                    mesh.indices.push_back(vertexCount    ); mesh.indices.push_back(vertexCount + 1); mesh.indices.push_back(vertexCount + 3);
                    mesh.indices.push_back(vertexCount + 1); mesh.indices.push_back(vertexCount + 2); mesh.indices.push_back(vertexCount + 3);

                    vertexCount = mesh.vertices.size();

                    break;
                }

                // Left preview
                if(cursorPos.x >= window->windowXPos + WINDOW_MARGIN &&
                   cursorPos.x <= window->windowXPos + WINDOW_MARGIN + WINDOW_DOCK_HIGHLIGHT_SIZE &&
                   cursorPos.y >= windowYMid - WINDOW_DOCK_HIGHLIGHT_SIZE &&
                   cursorPos.y <= windowYMid + WINDOW_DOCK_HIGHLIGHT_SIZE) {
                    size_t newWidth = window->windowWidth >> 1;
                    size_t newHeight = window->windowHeight;
                    newWidth = (newWidth < WINDOW_MIN_SIZE) ? WINDOW_MIN_SIZE : newWidth;

                    mesh.vertices.push_back({ { (float32_t)window->windowXPos,            (float32_t)window->windowYPos             }, { 0.f, 0.f }, { redDockPrev, greenDockPrev, blueDockPrev, 1.f } });
                    mesh.vertices.push_back({ { (float32_t)window->windowXPos + newWidth, (float32_t)window->windowYPos             }, { 0.f, 0.f }, { redDockPrev, greenDockPrev, blueDockPrev, 1.f } });
                    mesh.vertices.push_back({ { (float32_t)window->windowXPos + newWidth, (float32_t)window->windowYPos + newHeight }, { 0.f, 0.f }, { redDockPrev, greenDockPrev, blueDockPrev, 1.f } });
                    mesh.vertices.push_back({ { (float32_t)window->windowXPos,            (float32_t)window->windowYPos + newHeight }, { 0.f, 0.f }, { redDockPrev, greenDockPrev, blueDockPrev, 1.f } });

                    mesh.indices.push_back(vertexCount    ); mesh.indices.push_back(vertexCount + 1); mesh.indices.push_back(vertexCount + 3);
                    mesh.indices.push_back(vertexCount + 1); mesh.indices.push_back(vertexCount + 2); mesh.indices.push_back(vertexCount + 3);

                    vertexCount = mesh.vertices.size();

                    break;
                }

                // Right preview
                if(cursorPos.x >= window->windowXPos + window->windowWidth - WINDOW_MARGIN - WINDOW_DOCK_HIGHLIGHT_SIZE &&
                   cursorPos.x <= window->windowXPos + window->windowWidth - WINDOW_MARGIN &&
                   cursorPos.y >= windowYMid - WINDOW_DOCK_HIGHLIGHT_SIZE &&
                   cursorPos.y <= windowYMid + WINDOW_DOCK_HIGHLIGHT_SIZE) {
                    size_t newWidth = window->windowWidth >> 1;
                    size_t newHeight = window->windowHeight;
                    newWidth = (newWidth < WINDOW_MIN_SIZE) ? WINDOW_MIN_SIZE : newWidth;

                    mesh.vertices.push_back({ { (float32_t)window->windowXPos + newWidth,     (float32_t)window->windowYPos             }, { 0.f, 0.f }, { redDockPrev, greenDockPrev, blueDockPrev, 1.f } });
                    mesh.vertices.push_back({ { (float32_t)window->windowXPos + 2 * newWidth, (float32_t)window->windowYPos             }, { 0.f, 0.f }, { redDockPrev, greenDockPrev, blueDockPrev, 1.f } });
                    mesh.vertices.push_back({ { (float32_t)window->windowXPos + 2 * newWidth, (float32_t)window->windowYPos + newHeight }, { 0.f, 0.f }, { redDockPrev, greenDockPrev, blueDockPrev, 1.f } });
                    mesh.vertices.push_back({ { (float32_t)window->windowXPos + newWidth,     (float32_t)window->windowYPos + newHeight }, { 0.f, 0.f }, { redDockPrev, greenDockPrev, blueDockPrev, 1.f } });

                    mesh.indices.push_back(vertexCount    ); mesh.indices.push_back(vertexCount + 1); mesh.indices.push_back(vertexCount + 3);
                    mesh.indices.push_back(vertexCount + 1); mesh.indices.push_back(vertexCount + 2); mesh.indices.push_back(vertexCount + 3);

                    vertexCount = mesh.vertices.size();

                    break;
                }
                
                // Create the docking highlights
                
                // Top highlight
                mesh.vertices.push_back({ { (float32_t)windowXMid - WINDOW_DOCK_HIGHLIGHT_SIZE, (float32_t)window->windowYPos + 2 * WINDOW_MARGIN + WINDOW_TOP_BAR_HEIGHT                              }, { 0.f, 0.f }, { redDock, greenDock, blueDock, 1.f } });
                mesh.vertices.push_back({ { (float32_t)windowXMid + WINDOW_DOCK_HIGHLIGHT_SIZE, (float32_t)window->windowYPos + 2 * WINDOW_MARGIN + WINDOW_TOP_BAR_HEIGHT                              }, { 0.f, 0.f }, { redDock, greenDock, blueDock, 1.f } });
                mesh.vertices.push_back({ { (float32_t)windowXMid + WINDOW_DOCK_HIGHLIGHT_SIZE, (float32_t)window->windowYPos + 2 * WINDOW_MARGIN + WINDOW_TOP_BAR_HEIGHT + WINDOW_DOCK_HIGHLIGHT_SIZE }, { 0.f, 0.f }, { redDock, greenDock, blueDock, 1.f } });
                mesh.vertices.push_back({ { (float32_t)windowXMid - WINDOW_DOCK_HIGHLIGHT_SIZE, (float32_t)window->windowYPos + 2 * WINDOW_MARGIN + WINDOW_TOP_BAR_HEIGHT + WINDOW_DOCK_HIGHLIGHT_SIZE }, { 0.f, 0.f }, { redDock, greenDock, blueDock, 1.f } });

                mesh.indices.push_back(vertexCount    ); mesh.indices.push_back(vertexCount + 1); mesh.indices.push_back(vertexCount + 3);
                mesh.indices.push_back(vertexCount + 1); mesh.indices.push_back(vertexCount + 2); mesh.indices.push_back(vertexCount + 3);

                vertexCount = mesh.vertices.size();

                // Bottom highlight
                mesh.vertices.push_back({ { (float32_t)windowXMid - WINDOW_DOCK_HIGHLIGHT_SIZE, (float32_t)window->windowYPos + window->windowHeight - WINDOW_MARGIN - WINDOW_DOCK_HIGHLIGHT_SIZE }, { 0.f, 0.f }, { redDock, greenDock, blueDock, 1.f } });
                mesh.vertices.push_back({ { (float32_t)windowXMid + WINDOW_DOCK_HIGHLIGHT_SIZE, (float32_t)window->windowYPos + window->windowHeight - WINDOW_MARGIN - WINDOW_DOCK_HIGHLIGHT_SIZE }, { 0.f, 0.f }, { redDock, greenDock, blueDock, 1.f } });
                mesh.vertices.push_back({ { (float32_t)windowXMid + WINDOW_DOCK_HIGHLIGHT_SIZE, (float32_t)window->windowYPos + window->windowHeight - WINDOW_MARGIN                              }, { 0.f, 0.f }, { redDock, greenDock, blueDock, 1.f } });
                mesh.vertices.push_back({ { (float32_t)windowXMid - WINDOW_DOCK_HIGHLIGHT_SIZE, (float32_t)window->windowYPos + window->windowHeight - WINDOW_MARGIN                              }, { 0.f, 0.f }, { redDock, greenDock, blueDock, 1.f } });

                mesh.indices.push_back(vertexCount    ); mesh.indices.push_back(vertexCount + 1); mesh.indices.push_back(vertexCount + 3);
                mesh.indices.push_back(vertexCount + 1); mesh.indices.push_back(vertexCount + 2); mesh.indices.push_back(vertexCount + 3);

                vertexCount = mesh.vertices.size();

                // Left highlight
                mesh.vertices.push_back({ { (float32_t)window->windowXPos + WINDOW_MARGIN,                              (float32_t)windowYMid - WINDOW_DOCK_HIGHLIGHT_SIZE }, { 0.f, 0.f }, { redDock, greenDock, blueDock, 1.f } });
                mesh.vertices.push_back({ { (float32_t)window->windowXPos + WINDOW_MARGIN + WINDOW_DOCK_HIGHLIGHT_SIZE, (float32_t)windowYMid - WINDOW_DOCK_HIGHLIGHT_SIZE }, { 0.f, 0.f }, { redDock, greenDock, blueDock, 1.f } });
                mesh.vertices.push_back({ { (float32_t)window->windowXPos + WINDOW_MARGIN + WINDOW_DOCK_HIGHLIGHT_SIZE, (float32_t)windowYMid + WINDOW_DOCK_HIGHLIGHT_SIZE }, { 0.f, 0.f }, { redDock, greenDock, blueDock, 1.f } });
                mesh.vertices.push_back({ { (float32_t)window->windowXPos + WINDOW_MARGIN,                              (float32_t)windowYMid + WINDOW_DOCK_HIGHLIGHT_SIZE }, { 0.f, 0.f }, { redDock, greenDock, blueDock, 1.f } });

                mesh.indices.push_back(vertexCount    ); mesh.indices.push_back(vertexCount + 1); mesh.indices.push_back(vertexCount + 3);
                mesh.indices.push_back(vertexCount + 1); mesh.indices.push_back(vertexCount + 2); mesh.indices.push_back(vertexCount + 3);

                vertexCount = mesh.vertices.size();

                // Right highlight
                mesh.vertices.push_back({ { (float32_t)window->windowXPos + window->windowWidth - WINDOW_MARGIN - WINDOW_DOCK_HIGHLIGHT_SIZE, (float32_t)windowYMid - WINDOW_DOCK_HIGHLIGHT_SIZE }, { 0.f, 0.f }, { redDock, greenDock, blueDock, 1.f } });
                mesh.vertices.push_back({ { (float32_t)window->windowXPos + window->windowWidth - WINDOW_MARGIN,                              (float32_t)windowYMid - WINDOW_DOCK_HIGHLIGHT_SIZE }, { 0.f, 0.f }, { redDock, greenDock, blueDock, 1.f } });
                mesh.vertices.push_back({ { (float32_t)window->windowXPos + window->windowWidth - WINDOW_MARGIN,                              (float32_t)windowYMid + WINDOW_DOCK_HIGHLIGHT_SIZE }, { 0.f, 0.f }, { redDock, greenDock, blueDock, 1.f } });
                mesh.vertices.push_back({ { (float32_t)window->windowXPos + window->windowWidth - WINDOW_MARGIN - WINDOW_DOCK_HIGHLIGHT_SIZE, (float32_t)windowYMid + WINDOW_DOCK_HIGHLIGHT_SIZE }, { 0.f, 0.f }, { redDock, greenDock, blueDock, 1.f } });

                mesh.indices.push_back(vertexCount    ); mesh.indices.push_back(vertexCount + 1); mesh.indices.push_back(vertexCount + 3);
                mesh.indices.push_back(vertexCount + 1); mesh.indices.push_back(vertexCount + 2); mesh.indices.push_back(vertexCount + 3);

                vertexCount = mesh.vertices.size();

                break;
            }

        return mesh;
    }
}