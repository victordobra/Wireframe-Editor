#include "WireframeEngineEditor.hpp"

namespace wfe::editor {
    void GenerateEditorCallbacks(EditorCallbacks& callbacks) {
        callbacks.loadWorkspace = LoadWorkspace;
        callbacks.saveWorkspace = SaveWorkspace;
        callbacks.renderWindows = RenderWindows;

        callbacks.getLoadCallback = GetLoadCallback;
        callbacks.setLoadCallback = SetLoadCallback;
        callbacks.getSaveCallback = GetSaveCallback;
        callbacks.setSaveCallback = SetSaveCallback;

        callbacks.getWorkspaceDir = GetWorkspaceDir;
        callbacks.setWorkspaceDir = SetWorkspaceDir;

        callbacks.getMainWindowWidth = GetMainWindowWidth;
        callbacks.getMainWindowHeight = GetMainWindowHeight;

        callbacks.getMainWindowName = GetMainWindowName;
        callbacks.setMainWindowName = SetMainWindowName;
        
        callbacks.openFolderDialog = OpenFolderDialog;
        callbacks.copyFolder = CopyFolder;

        callbacks.createDevice = CreateDevice;
        callbacks.deleteDevice = DeleteDevice;
        
        callbacks.getVulkanAllocator = GetVulkanAllocator;
        callbacks.getVulkanInstance = GetVulkanInstance;
        callbacks.getPhysicalDevice = GetPhysicalDevice;
        callbacks.getCommandPool = GetCommandPool;
        callbacks.getDevice = GetDevice;
        callbacks.getSurface = GetSurface;
        callbacks.getGraphicsQueue = GetGraphicsQueue;
        callbacks.getPresentQueue = GetPresentQueue;
        callbacks.getPhysicalDeviceProperties = GetPhysicalDeviceProperties;
        callbacks.getPhysicalDeviceFeatures = GetPhysicalDeviceFeatures;
        callbacks.areValidationLayersEnabled = AreValidationLayersEnabled;
        callbacks.enableValidationLayers = EnableValidationLayers;
        callbacks.disableValidationLayers = DisableValidationLayers;
        
        callbacks.getSwapChainSupport = GetSwapChainSupport;
        callbacks.findPhysicalQueueFamilies = FindPhysicalQueueFamilies;

        callbacks.findMemoryType = FindMemoryType;
        callbacks.findSupportedFormat = FindSupportedFormat;

        callbacks.createBuffer = CreateBuffer;
        callbacks.createImage = CreateImage;
        callbacks.beginSingleTimeCommands = BeginSingleTimeCommands;
        callbacks.endSingleTimeCommands = EndSingleTimeCommands;
        callbacks.copyBuffer = CopyBuffer;
        callbacks.copyImage = CopyImage;
        callbacks.copyBufferToImage = CopyBufferToImage;
        callbacks.copyImageToBuffer = CopyImageToBuffer;
        callbacks.transitionImageLayout = TransitionImageLayout;

        callbacks.padUniformBufferSize = PadUniformBufferSize;

        callbacks.createImGuiPipeline = CreateImGuiPipeline;
        callbacks.deleteImGuiPipeline = DeleteImGuiPipeline;
        callbacks.drawImGui = DrawImGui;
        
        callbacks.getImGuiPipeline = GetImGuiPipeline;
        callbacks.getImGuiPipelineLayout = GetImGuiPipelineLayout;
        callbacks.getImGuiCommandBuffer = GetImGuiCommandBuffer;

        callbacks.createSwapChain = CreateSwapChain;
        callbacks.deleteSwapChain = DeleteSwapChain;
        callbacks.recreateSwapChain = RecreateSwapChain;
        
        callbacks.getFrameBuffer = GetFrameBuffer;
        callbacks.getRenderPass = GetRenderPass;
        callbacks.getImageView = GetImageView;
        callbacks.getImageCount = GetImageCount;
        callbacks.getSwapChainImageFormat = GetSwapChainImageFormat;
        callbacks.getSwapChainExtent = GetSwapChainExtent;
        callbacks.getSwapChainWidth = GetSwapChainWidth;
        callbacks.getSwapChainHeight = GetSwapChainHeight;
        callbacks.getCurrentFrame = GetCurrentFrame;

        callbacks.extentAspectRatio = ExtentAspectRatio;
        callbacks.findDepthFormat = FindDepthFormat;

        callbacks.acquireNextImage = AcquireNextImage;
        callbacks.submitCommandBuffers = SubmitCommandBuffers;

        callbacks.loadEditorProperties = LoadEditorProperties;
        callbacks.saveEditorProperties = SaveEditorProperties;

        callbacks.getDefaultTemplateLocation = GetDefaultTemplateLocation;
        callbacks.setDefaultTemplateLocation = SetDefaultTemplateLocation;
        callbacks.getDefaultProjectLocation = GetDefaultProjectLocation;
        callbacks.setDefaultProjectLocation = SetDefaultProjectLocation;
        callbacks.getTemplateLocations = GetTemplateLocations;

        callbacks.createNewProject = CreateNewProject;

        callbacks.windowTypeMap = WindowType::windowTypes;
    }
}