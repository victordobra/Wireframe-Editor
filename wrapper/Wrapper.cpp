#include "WireframeEngineEditor.hpp"

namespace wfe::editor {
    // Variables
    constinit map<string, WindowType> windowTypesPlaceholder;
    constinit map<string, WindowType>* WindowType::windowTypes = &windowTypesPlaceholder;

    EditorCallbacks editorCallbacks;

    // Internal helper functions
    static void SetCallbacks(const EditorCallbacks& callbacks) {
        // Set the new editor callbacks
        editorCallbacks = callbacks;
    }

    // Public functions
    void LoadWorkspace() {
        editorCallbacks.loadWorkspace();
    }
    void SaveWorkspace() {
        editorCallbacks.saveWorkspace();
    }
    void RenderWindows() {
        editorCallbacks.renderWindows();
    }

    LoadCallback GetLoadCallback() {
        return editorCallbacks.getLoadCallback();
    }
    void SetLoadCallback(LoadCallback newLoadCallback) {
        if(editorCallbacks.setLoadCallback)
            editorCallbacks.setLoadCallback(newLoadCallback);
    }
    SaveCallback GetSaveCallback() {
        return editorCallbacks.getSaveCallback();
    }
    void SetSaveCallback(SaveCallback newSaveCallback) {
        if(editorCallbacks.setSaveCallback)
            editorCallbacks.setSaveCallback(newSaveCallback);
    }
    CloseCallback GetCloseCallback() {
        return editorCallbacks.getCloseCallback();
    }
    void SetCloseCallback(CloseCallback newCloseCallback) {
        if(editorCallbacks.setCloseCallback)
            editorCallbacks.setCloseCallback(newCloseCallback);
    }

    string GetWorkspaceDir() {
        return editorCallbacks.getWorkspaceDir();
    }
    void SetWorkspaceDir(const string& newWorkspaceDir, bool8_t removeFromRecents) {
        editorCallbacks.setWorkspaceDir(newWorkspaceDir, removeFromRecents);
    }
    void CloseWorkspace() {
        editorCallbacks.closeWorkspace();
    }
    
    vector<Event> GetEvents() {
        return editorCallbacks.getEvents();
    }
    vector<Event> GetEventsOfType(EventType type) {
        return editorCallbacks.getEventsOfType(type);
    }

    void AddEvent(Event event) {
        editorCallbacks.addEvent(event);
    }

    void CloseApplication(int32_t returnCode) {
        editorCallbacks.closeApplication(returnCode);
    }
    bool8_t IsInsideEditor() {
        if(editorCallbacks.isInsideEditor)
            return editorCallbacks.isInsideEditor();
        return false;
    }

    size_t GetMainWindowWidth() {
        return editorCallbacks.getMainWindowWidth();
    }
    size_t GetMainWindowHeight() {
        return editorCallbacks.getMainWindowHeight();
    }

    string GetMainWindowName() {
        return editorCallbacks.getMainWindowName();
    }
    void SetMainWindowName(const string& newName) {
        editorCallbacks.setMainWindowName(newName);
    }

    void CreateImGui() {
        editorCallbacks.createImGui();
    }
    void DeleteImGui() {
        editorCallbacks.deleteImGui();
    }
    void PorcessImGuiEvents() {
        editorCallbacks.processImGuiEvents();
    }

    ImGuiContext* GetImGuiContext() {
        return editorCallbacks.getImGuiContext();
    }
    ImGuiIO* GetImGuiIO() {
        return editorCallbacks.getImGuiIO();
    }
    ImGuiStyle* GetImGuiStyle() {
        return editorCallbacks.getImGuiStyle();
    }

    ImFont* GetImGuiNormalFont() {
        return editorCallbacks.getImGuiNormalFont();
    }
    ImFont* GetImGuiBoldFont() {
        return editorCallbacks.getImGuiBoldFont();
    }
    ImFont* GetImGuiItalicFont() {
        return editorCallbacks.getImGuiItalicFont();
    }

    void CreatePlatform() {
        editorCallbacks.createPlatform();
    }
    void DeletePlatform() {
        editorCallbacks.deletePlatform();
    }
    void PollPlatformEvents() {
        editorCallbacks.pollPlatformEvents();
    }

    PlatformInfo* GetPlatformInfo() {
        return editorCallbacks.getPlatformInfo();
    }

    bool8_t LocationExists(const string& location) {
        return editorCallbacks.locationExists(location);
    }
    void CopyFiles(const string& srcFile, const string& dstFile, bool8_t folders) {
        editorCallbacks.copyFiles(srcFile, dstFile, folders);
    }
    string OpenFileDialog(const string& name, bool8_t& canceled, const string& startingLocation, bool8_t folders) {
        return editorCallbacks.openFileDialog(name, canceled, startingLocation, folders);
    }

    VkResult CreatePlatformSurface(VkInstance instance, const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface) {
        return editorCallbacks.createPlatformSurface(instance, pAllocator, pSurface);
    }

    void CreateDevice() {
        editorCallbacks.createDevice();
    }
    void DeleteDevice() {
        editorCallbacks.deleteDevice();
    }

    const VkAllocationCallbacks* GetVulkanAllocator() {
        return editorCallbacks.getVulkanAllocator();
    }
    VkInstance GetVulkanInstance() {
        return editorCallbacks.getVulkanInstance();
    }
    VkPhysicalDevice GetPhysicalDevice() {
        return editorCallbacks.getPhysicalDevice();
    }
    VkCommandPool GetCommandPool() {
        return editorCallbacks.getCommandPool();
    }
    VkDevice GetDevice() {
        return editorCallbacks.getDevice();
    }
    VkSurfaceKHR GetSurface() {
        return editorCallbacks.getSurface();
    }
    VkQueue GetGraphicsQueue() {
        return editorCallbacks.getGraphicsQueue();
    }
    VkQueue GetPresentQueue() {
        return editorCallbacks.getPresentQueue();
    }
    const VkPhysicalDeviceProperties& GetPhysicalDeviceProperties() {
        return editorCallbacks.getPhysicalDeviceProperties();
    }
    const VkPhysicalDeviceFeatures& GetPhysicalDeviceFeatures() {
        return editorCallbacks.getPhysicalDeviceFeatures();
    }
    bool8_t AreValidationLayersEnabled() {
        return editorCallbacks.areValidationLayersEnabled();
    }
    void EnableValidationLayers() {
        editorCallbacks.enableValidationLayers();
    }
    void DisableValicationLayers() {
        editorCallbacks.disableValidationLayers();
    }

    SwapChainSupportDetails GetSwapChainSupport() {
        return editorCallbacks.getSwapChainSupport();
    }
    QueueFamilyIndices FindPhysicalQueueFamilies() {
        return editorCallbacks.findPhysicalQueueFamilies();
    }

    uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
        return editorCallbacks.findMemoryType(typeFilter, properties);
    }
    VkFormat FindSupportedFormat(const vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
        return editorCallbacks.findSupportedFormat(candidates, tiling, features);
    }

    void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {
        editorCallbacks.createBuffer(size, usage, properties, buffer, bufferMemory);
    }
    void CreateImage(const VkImageCreateInfo& imageInfo, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory) {
        editorCallbacks.createImage(imageInfo, properties, image, imageMemory);
    }
    VkCommandBuffer BeginSingleTimeCommands() {
        return editorCallbacks.beginSingleTimeCommands();
    }
    void EndSingleTimeCommands(VkCommandBuffer commandBuffer) {
        editorCallbacks.endSingleTimeCommands(commandBuffer);
    }
    void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkCommandBuffer commandBuffer) {
        editorCallbacks.copyBuffer(srcBuffer, dstBuffer, size, commandBuffer);
    }
    void CopyImage(VkImage srcImage, VkImage dstImage, uint32_t width, uint32_t height, uint32_t srcLayerCount, uint32_t dstLayerCount, VkCommandBuffer commandBuffer) {
        editorCallbacks.copyImage(srcImage, dstImage, width, height, srcLayerCount, dstLayerCount, commandBuffer);
    }
    void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount, VkCommandBuffer commandBuffer) {
        editorCallbacks.copyBufferToImage(buffer, image, width, height, layerCount, commandBuffer);
    }
    void CopyImageToBuffer(VkImage image, VkBuffer buffer, uint32_t width, uint32_t height, uint32_t layerCount, VkCommandBuffer commandBuffer) {
        editorCallbacks.copyImageToBuffer(image, buffer, width, height, layerCount, commandBuffer);
    }
    void TransitionImageLayout(VkImage image, VkImageLayout srcLayout, VkImageLayout dstLayout, VkFormat format, VkCommandBuffer commandBuffer) {
        editorCallbacks.transitionImageLayout(image, srcLayout, dstLayout, format, commandBuffer);
    }
    
    VkDeviceSize PadUniformBufferSize(VkDeviceSize originalSize) {
        return editorCallbacks.padUniformBufferSize(originalSize);
    }

    void CreateImGuiPipeline() {
        editorCallbacks.createImGuiPipeline();
    }
    void DeleteImGuiPipeline() {
        editorCallbacks.deleteImGuiPipeline();
    }
    void DrawImGui() {
        editorCallbacks.drawImGui();
    }

    VkPipeline GetImGuiPipeline() {
        return editorCallbacks.getImGuiPipeline();
    }
    VkPipelineLayout GetImGuiPipelineLayout() {
        return editorCallbacks.getImGuiPipelineLayout();
    }
    VkCommandBuffer GetImGuiCommandBuffer() {
        return editorCallbacks.getImGuiCommandBuffer();
    }

    void CreateSwapChain() {
        editorCallbacks.createSwapChain();
    }
    void DeleteSwapChain() {
        editorCallbacks.deleteSwapChain();
    }
    void RecreateSwapChain() {
        editorCallbacks.recreateSwapChain();
    }
    VkFramebuffer GetFrameBuffer(size_t index) {
        return editorCallbacks.getFrameBuffer(index);
    }
    VkRenderPass GetRenderPass() {
        return editorCallbacks.getRenderPass();
    }
    VkImageView GetImageView(size_t index) {
        return editorCallbacks.getImageView(index);
    }
    size_t GetImageCount() {
        return editorCallbacks.getImageCount();
    }
    VkFormat GetSwapChainImageFormat() {
        return editorCallbacks.getSwapChainImageFormat();
    }
    VkExtent2D GetSwapChainExtent() {
        return editorCallbacks.getSwapChainExtent();
    }
    size_t GetSwapChainWidth() {
        return editorCallbacks.getSwapChainWidth();
    }
    size_t GetSwapChainHeight() {
        return editorCallbacks.getSwapChainHeight();
    }
    uint32_t GetCurrentFrame() {
        return editorCallbacks.getCurrentFrame();
    }

    float32_t ExtentAspectRatio() {
        return editorCallbacks.extentAspectRatio();
    }
    VkFormat FindDepthFormat() {
        return editorCallbacks.findDepthFormat();
    }

    VkResult AcquireNextImage(uint32_t* imageIndex) {
        return editorCallbacks.acquireNextImage(imageIndex);
    }
    VkResult SubmitCommandBuffers(const VkCommandBuffer* buffers, uint32_t* imageIndex, uint32_t bufferCount) {
        return editorCallbacks.submitCommandBuffers(buffers, imageIndex, bufferCount);
    }

    void LoadEditorProperties() {
        editorCallbacks.loadEditorProperties();
    }
    void SaveEditorProperties() {
        editorCallbacks.saveEditorProperties();
    }

    string GetDefaultTemplateLocation() {
        return editorCallbacks.getDefaultTemplateLocation();
    }
    void SetDefaultTemplateLocation(const string& newLocation) {
        editorCallbacks.setDefaultTemplateLocation(newLocation);
    }
    string GetDefaultProjectLocation() {
        return editorCallbacks.getDefaultProjectLocation();
    }
    void SetDefaultProjectLocation(const string& newLocation) {
        editorCallbacks.setDefaultProjectLocation(newLocation);
    }
    vector<string> GetTemplateLocations() {
        return editorCallbacks.getTemplateLocations();
    }

    void CreateNewProject() {
        editorCallbacks.createNewProject();
    }
}

extern "C" {
    void SetCallbacks(const wfe::editor::EditorCallbacks& callbacks) {
        wfe::editor::SetCallbacks(callbacks);
    }
}