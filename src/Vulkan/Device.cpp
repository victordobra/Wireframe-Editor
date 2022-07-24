#include "Vulkan/Device.hpp"
#include "ProjectInfo.hpp"
#include "MainWindow/MainWindow.hpp"

namespace wfe::editor {
    // Constants
    const vector<const char_t*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
    const vector<const char_t*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

#ifdef PLATFORM_WINDOWS
#ifdef NDEBUG
    const vector<const char_t*> requiredExtensions = { VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_WIN32_SURFACE_EXTENSION_NAME };
#else
    const vector<const char_t*> requiredExtensions = { VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_WIN32_SURFACE_EXTENSION_NAME, VK_EXT_DEBUG_UTILS_EXTENSION_NAME };
#endif
#endif

    // Variables
    VkAllocationCallbacks* allocator = nullptr;

    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;
    VkSurfaceKHR surface;
    VkPhysicalDevice physicalDevice;
    VkPhysicalDeviceProperties physicalDeviceProperties;
    VkDevice device;
    VkQueue graphicsQueue, presentQueue;
    VkCommandPool commandPool;

    // Debug callback
    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
        switch(messageSeverity) {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            console::OutErrorFunction((string)"Validation layer: " + pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            console::OutWarningFunction((string)"Validation layer: " + pCallbackData->pMessage);
            break;
        default:
            console::OutMessageFunction((string)"Validation layer: " + pCallbackData->pMessage);
            break;
        }

        return VK_FALSE;
    }

    // Internal helper functions
    static VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pMessenger) {
        auto createFunction = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if(createFunction)
            return createFunction(instance, pCreateInfo, pAllocator, pMessenger);
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
    static void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT messenger, const VkAllocationCallbacks* pAllocator) {
        if(!enableValidationLayers)
            return;

        auto destroyFunction = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        if(destroyFunction)
            destroyFunction(instance, messenger, pAllocator);
    }

    static void CheckRequiredInstanceExtensions() {
        // Get all of the available extensions
        uint32_t availableExtensionCount;
        vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, nullptr);
        vector<VkExtensionProperties> availableExtensions(availableExtensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, availableExtensions.data());

        // Add all of the available extension names in an unordered set
        unordered_set<string> availableExtensionSet;
        for(const auto& extension : availableExtensions)
            availableExtensionSet.insert(extension.extensionName);

        // Find all of the missing extensions
        string missingExtensions = "";

        for(const auto* extension : requiredExtensions)
            if(!availableExtensionSet.count(extension))
                missingExtensions += (string)extension + "; ";
        
        // Output an error if at least one of the extensions is missing
        if(missingExtensions.length())
            console::OutFatalError((string)"Failed to find all required extensions! Missing extensions: " + missingExtensions, 1);
    }
    static void CheckValidationLayerSupport() {
        // If validation layers shouldn't be enabled, exit the function
        if(!enableValidationLayers)
            return;

        // Get all of the available layers
        uint32_t availableLayerCount;
        vkEnumerateInstanceLayerProperties(&availableLayerCount, nullptr);
        vector<VkLayerProperties> availableLayers(availableLayerCount);
        vkEnumerateInstanceLayerProperties(&availableLayerCount, availableLayers.data());

        // Put all of the available layers in an unordered set
        unordered_set<string> availableLayerSet;
        for(const auto& layer : availableLayers)
            availableLayerSet.insert(layer.layerName);
        
        // Find all of the missing layers
        string missingLayers = "";

        for(const auto* layer : validationLayers)
            if(!availableLayerSet.count(layer))
                missingLayers += (string)layer + "; ";
        
        // Output an error if at least one of the layers is missing
        if(missingLayers.length())
            console::OutFatalError((string)"Failed to find all required validation layers! Missing layers: " + missingLayers, 1);
    }
    static QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice physicalDevice) {
        QueueFamilyIndices indices{};

        // Find all queue families
        uint32_t queueFamilyCount;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
        vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

        // Loop through all of the queue families and check what they support
        for(uint32_t i = 0; i < queueFamilyCount && !indices.IsComplete(); ++i) {
            if(queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indices.graphicsFamily = i;
                indices.graphicsFamilyHasValue = true;
            }

            VkBool32 presentSupport;
            auto result = vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);
            if(result != VK_SUCCESS)
                console::OutFatalError((string)"Failed to get GPU surface support! Error code: " + VkResultToString(result), 1);
            
            if(presentSupport) {
                indices.presentFamily = i;
                indices.presentFamilyHasValue = true;
            }
        }

        return indices;
    }
    static SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice physicalDevice) {
        SwapChainSupportDetails supportDetails;

        /// Get the surface capabilities
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &supportDetails.capabilities);

        // Get the surface formats
        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);

        if(formatCount) {
            supportDetails.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, supportDetails.formats.data());
        }

        // Get the present modes
        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);

        if(presentModeCount) {
            supportDetails.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, supportDetails.presentModes.data());
        }

        return supportDetails;
    }
    static bool8_t CheckDeviceExtensionSupport(VkPhysicalDevice physicalDevice) {
        // Get all device extensions
        uint32_t availableExtensionCount;
        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &availableExtensionCount, nullptr);
        vector<VkExtensionProperties> availableExtensions(availableExtensionCount);
        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &availableExtensionCount, availableExtensions.data());

        // Put all available extensions in an unordered set
        unordered_set<string> availableExtensionSet;
        for(const auto& extension : availableExtensions)
            availableExtensionSet.insert(extension.extensionName);
        
        // Return false if at least one extension is missing
        for(const auto* extension : requiredExtensions)
            if(!availableExtensionSet.count(extension))
                return false;
        
        return true;
    }
    static bool8_t IsDeviceSuitable(VkPhysicalDevice physicalDevice) {
        QueueFamilyIndices indices = FindQueueFamilies(physicalDevice);
        bool8_t extensionsSupported = CheckDeviceExtensionSupport(physicalDevice);

        bool8_t swapChainAdequate = false;
        if(extensionsSupported) {
            SwapChainSupportDetails supportDetails = QuerySwapChainSupport(physicalDevice);
            swapChainAdequate = !supportDetails.formats.empty() && !supportDetails.presentModes.empty();
        }

        return indices.IsComplete() && extensionsSupported && swapChainAdequate;
    }
    static void SetStageAndAccess(VkImageLayout layout, VkAccessFlags& accessMask, VkPipelineStageFlags& stage) {
        // Check for every supported layout
        switch(layout) {
        case VK_IMAGE_LAYOUT_UNDEFINED:
            accessMask = 0;
            stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            break;
        case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
            accessMask = VK_ACCESS_TRANSFER_READ_BIT;
            stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            break;
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            accessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            break;
        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
            accessMask = VK_ACCESS_SHADER_READ_BIT;
            stage = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;
            break;
        default:
            console::OutFatalError("Unsupported image layout!", 1);
            break;
        }
    }

    static void CreateInstance() {
        // Check if the instance has the required extensions
        CheckRequiredInstanceExtensions();

        // Set the application info
        VkApplicationInfo appInfo;

        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pNext = nullptr;
        appInfo.pApplicationName = PROJECT_NAME;
        appInfo.applicationVersion = VK_MAKE_VERSION(PROJECT_VERSION_MAJOR, PROJECT_VERSION_MINOR, PROJECT_VERSION_PATCH);
        appInfo.pEngineName = PROJECT_NAME;
        appInfo.engineVersion = VK_MAKE_VERSION(PROJECT_VERSION_MAJOR, PROJECT_VERSION_MINOR, PROJECT_VERSION_PATCH);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        // Set the debug messenger create info for creating the instance
        VkDebugUtilsMessengerCreateInfoEXT debugMessengerInfo;

        debugMessengerInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debugMessengerInfo.pNext = nullptr;
        debugMessengerInfo.flags = 0;
        debugMessengerInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        debugMessengerInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debugMessengerInfo.pfnUserCallback = DebugCallback;
        debugMessengerInfo.pUserData = nullptr;

        // Set the instance create info
        VkInstanceCreateInfo createInfo;
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.pApplicationInfo = &appInfo;

        if(enableValidationLayers) {
            createInfo.enabledLayerCount = validationLayers.size();
            createInfo.ppEnabledLayerNames = validationLayers.data();

            createInfo.pNext = &debugMessengerInfo;
        } else {
            createInfo.enabledLayerCount = 0;
            createInfo.ppEnabledLayerNames = nullptr;
        }

        createInfo.enabledExtensionCount = requiredExtensions.size();
        createInfo.ppEnabledExtensionNames = requiredExtensions.data();

        // Create the instance
        auto result = vkCreateInstance(&createInfo, allocator, &instance);
        if(result != VK_SUCCESS)
            console::OutFatalError((string)"Failed to create instance! Error code: " + VkResultToString(result), 1);
        console::OutMessageFunction("Created Vulkan instance successfully.");
    }
    static void SetupDebugMessenger() {
        // If validation layers shouldn't be enabled, exit the function
        if(!enableValidationLayers)
            return;

        // Check for validation layer support
        CheckValidationLayerSupport();

        // Set the debug messenger create info
        VkDebugUtilsMessengerCreateInfoEXT createInfo;

        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = DebugCallback;
        createInfo.pUserData = nullptr;

        // Create the debug messenger
        auto result = CreateDebugUtilsMessengerEXT(instance, &createInfo, allocator, &debugMessenger);
        if(result != VK_SUCCESS)
            console::OutFatalError((string)"Failed to create debug messenger! Error code: " + VkResultToString(result), 1);
        console::OutMessageFunction("Created Vulkan debug messenger successfully.");
    }
    static void CreateSurface() {
#ifdef PLATFORM_WINDOWS
        // Set the surface create info
        VkWin32SurfaceCreateInfoKHR createInfo;
        
        createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.hwnd = GetWindowHandle();
        createInfo.hinstance = GetWindowsInstance();

        // Create the surface
        auto result = vkCreateWin32SurfaceKHR(instance, &createInfo, allocator, &surface);
        if(result != VK_SUCCESS)
            console::OutFatalError((string)"Failed to create surface! Error code: " + VkResultToString(result), 1);
        console::OutMessageFunction("Created Vulkan window surface successfully.");
#endif
    }
    static void PickPhysicalDevice() {
        // Enumerate all physical devices
        uint32_t physicalDeviceCount;
        vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);
        vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
        vkEnumeratePhysicalDevices(instance,&physicalDeviceCount, physicalDevices.data());

        // Loop through all devices and pick the first one
        for(auto pDevice : physicalDevices)
            if(IsDeviceSuitable(pDevice)) {
                physicalDevice = pDevice;
                break;
            }

        if(physicalDevice == VK_NULL_HANDLE)
            console::OutFatalError("Failed to find a suitable GPU!", 1);
        
        // Output the device's name
        vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);
        console::OutMessageFunction((string)"Found suitable GPU: " + physicalDeviceProperties.deviceName + ".");
    }
    static void CreateLogicalDevice() {
        // Set the device queue create info
        QueueFamilyIndices indices = FindQueueFamilies(physicalDevice);
        float32_t queuePriority = 1.f;

        VkDeviceQueueCreateInfo queueCreateInfos[2];
        uint32_t queueCount = indices.graphicsFamily != indices.presentFamily + 1;

        queueCreateInfos[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfos[0].pNext = nullptr;
        queueCreateInfos[0].flags = 0;
        queueCreateInfos[0].queueFamilyIndex = indices.graphicsFamily;
        queueCreateInfos[0].queueCount = 1;
        queueCreateInfos[0].pQueuePriorities = &queuePriority;

        if(queueCount == 2) {
            queueCreateInfos[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfos[1].pNext = nullptr;
            queueCreateInfos[1].flags = 0;
            queueCreateInfos[1].queueFamilyIndex = indices.presentFamily;
            queueCreateInfos[1].queueCount = 1;
            queueCreateInfos[1].pQueuePriorities = &queuePriority;
        }

        // Set the physical device fratures
        VkPhysicalDeviceFeatures physicalDeviceFeatues;

        physicalDeviceFeatues.robustBufferAccess = VK_FALSE;
        physicalDeviceFeatues.fullDrawIndexUint32 = VK_FALSE;
        physicalDeviceFeatues.imageCubeArray = VK_FALSE;
        physicalDeviceFeatues.independentBlend = VK_FALSE;
        physicalDeviceFeatues.geometryShader = VK_FALSE;
        physicalDeviceFeatues.tessellationShader = VK_FALSE;
        physicalDeviceFeatues.sampleRateShading = VK_FALSE;
        physicalDeviceFeatues.dualSrcBlend = VK_FALSE;
        physicalDeviceFeatues.logicOp = VK_FALSE;
        physicalDeviceFeatues.multiDrawIndirect = VK_FALSE;
        physicalDeviceFeatues.drawIndirectFirstInstance = VK_FALSE;
        physicalDeviceFeatues.depthClamp = VK_FALSE;
        physicalDeviceFeatues.depthBiasClamp = VK_FALSE;
        physicalDeviceFeatues.fillModeNonSolid = VK_FALSE;
        physicalDeviceFeatues.depthBounds = VK_FALSE;
        physicalDeviceFeatues.wideLines = VK_FALSE;
        physicalDeviceFeatues.largePoints = VK_FALSE;
        physicalDeviceFeatues.alphaToOne = VK_FALSE;
        physicalDeviceFeatues.multiViewport = VK_FALSE;
        physicalDeviceFeatues.samplerAnisotropy = VK_FALSE;
        physicalDeviceFeatues.textureCompressionETC2 = VK_FALSE;
        physicalDeviceFeatues.textureCompressionASTC_LDR = VK_FALSE;
        physicalDeviceFeatues.textureCompressionBC = VK_FALSE;
        physicalDeviceFeatues.occlusionQueryPrecise = VK_FALSE;
        physicalDeviceFeatues.pipelineStatisticsQuery = VK_FALSE;
        physicalDeviceFeatues.vertexPipelineStoresAndAtomics = VK_FALSE;
        physicalDeviceFeatues.fragmentStoresAndAtomics = VK_FALSE;
        physicalDeviceFeatues.shaderTessellationAndGeometryPointSize = VK_FALSE;
        physicalDeviceFeatues.shaderImageGatherExtended = VK_FALSE;
        physicalDeviceFeatues.shaderStorageImageExtendedFormats = VK_FALSE;
        physicalDeviceFeatues.shaderStorageImageMultisample = VK_FALSE;
        physicalDeviceFeatues.shaderStorageImageReadWithoutFormat = VK_FALSE;
        physicalDeviceFeatues.shaderStorageImageWriteWithoutFormat = VK_FALSE;
        physicalDeviceFeatues.shaderUniformBufferArrayDynamicIndexing = VK_FALSE;
        physicalDeviceFeatues.shaderSampledImageArrayDynamicIndexing = VK_FALSE;
        physicalDeviceFeatues.shaderStorageBufferArrayDynamicIndexing = VK_FALSE;
        physicalDeviceFeatues.shaderStorageImageArrayDynamicIndexing = VK_FALSE;
        physicalDeviceFeatues.shaderClipDistance = VK_FALSE;
        physicalDeviceFeatues.shaderCullDistance = VK_FALSE;
        physicalDeviceFeatues.shaderFloat64 = VK_FALSE;
        physicalDeviceFeatues.shaderInt64 = VK_FALSE;
        physicalDeviceFeatues.shaderInt16 = VK_FALSE;
        physicalDeviceFeatues.shaderResourceResidency = VK_FALSE;
        physicalDeviceFeatues.shaderResourceMinLod = VK_FALSE;
        physicalDeviceFeatues.sparseBinding = VK_FALSE;
        physicalDeviceFeatues.sparseResidencyBuffer = VK_FALSE;
        physicalDeviceFeatues.sparseResidencyImage2D = VK_FALSE;
        physicalDeviceFeatues.sparseResidencyImage3D = VK_FALSE;
        physicalDeviceFeatues.sparseResidency2Samples = VK_FALSE;
        physicalDeviceFeatues.sparseResidency4Samples = VK_FALSE;
        physicalDeviceFeatues.sparseResidency8Samples = VK_FALSE;
        physicalDeviceFeatues.sparseResidency16Samples = VK_FALSE;
        physicalDeviceFeatues.sparseResidencyAliased = VK_FALSE;
        physicalDeviceFeatues.variableMultisampleRate = VK_FALSE;
        physicalDeviceFeatues.inheritedQueries = VK_FALSE;

        // Set the device create info
        VkDeviceCreateInfo createInfo;

        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.queueCreateInfoCount = queueCount;
        createInfo.pQueueCreateInfos = queueCreateInfos;
        
        if(enableValidationLayers) {
            createInfo.enabledLayerCount = (uint32_t)validationLayers.size();
            createInfo.ppEnabledLayerNames = validationLayers.data();
        } else {
            createInfo.enabledLayerCount = 0;
            createInfo.ppEnabledLayerNames = nullptr;
        }

        createInfo.enabledExtensionCount = (uint32_t)deviceExtensions.size();
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();
        createInfo.pEnabledFeatures = &physicalDeviceFeatues;

        // Create the device
        auto result = vkCreateDevice(physicalDevice, &createInfo, allocator, &device);
        if(result != VK_SUCCESS)
            console::OutFatalError((string)"Failed to create logical device! Error code: " + VkResultToString(result), 1);
        console::OutMessageFunction("Created Vulkan logical device successfully.");
        
        // Get the device queues
        vkGetDeviceQueue(device, indices.graphicsFamily, 0, &graphicsQueue);
        vkGetDeviceQueue(device, indices.presentFamily, 0, &presentQueue);
    }
    static void CreateCommandPool() {
        QueueFamilyIndices indices = FindQueueFamilies(physicalDevice);

        // Set the command pool create info
        VkCommandPoolCreateInfo createInfo;

        createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        createInfo.queueFamilyIndex = indices.graphicsFamily;

        auto result = vkCreateCommandPool(device, &createInfo, allocator, &commandPool);
        if(result != VK_SUCCESS)
            console::OutFatalError((string)"Failed to create command pool! Error code: " + VkResultToString(result), 1);
        console::OutMessageFunction("Created Vulkan command pool successfully.");
    }

    // Public functions
    void CreateDevice() {
        CreateInstance();
        SetupDebugMessenger();
        CreateSurface();
        PickPhysicalDevice();
        CreateLogicalDevice();
        CreateCommandPool();
    }
    void DeleteDevice() {
        vkDestroyCommandPool(device, commandPool, allocator);
        vkDestroyDevice(device, allocator);
        vkDestroySurfaceKHR(instance, surface, allocator);
        DestroyDebugUtilsMessengerEXT(instance, debugMessenger, allocator);
        vkDestroyInstance(instance, allocator);

        console::OutMessageFunction("Deleted Vulkan instance successfully.");
    }

    const VkAllocationCallbacks* GetVulkanAllocator() {
        return allocator;
    }
    VkInstance GetVulkanInstance() {
        return instance;
    }
    VkPhysicalDevice GetPhysicalDevice() {
        return physicalDevice;
    }
    VkCommandPool GetCommandPool() {
        return commandPool;
    }
    VkDevice GetDevice() {
        return device;
    }
    VkSurfaceKHR GetSurface() {
        return surface;
    }
    VkQueue GetGraphicsQueue() {
        return graphicsQueue;
    }
    VkQueue GetPresentQueue() {
        return presentQueue;
    }
    const VkPhysicalDeviceProperties& GetDeviceProperties() {
        return physicalDeviceProperties;
    }

    SwapChainSupportDetails GetSwapChainSupport() {
        return QuerySwapChainSupport(physicalDevice);
    }
    QueueFamilyIndices FindPhysicalQueueFamilies() {
        return FindQueueFamilies(physicalDevice);
    }

    uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
        // Get the physical device's memory properties
        VkPhysicalDeviceMemoryProperties memoryProperties;
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);

        for(uint32_t i = 0; i < memoryProperties.memoryTypeCount; ++i)
            if((typeFilter & (1 << i)) && (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
                return i;
        
        // No suitable memory types were found; throw an error
        console::OutFatalError("Failed to find suitable memory type!", 1);
        return 0;
    }
    VkFormat FindSupportedFormat(const vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
        for(const auto& candidate : candidates) {
            // Get the format properties
            VkFormatProperties formatProperties;
            vkGetPhysicalDeviceFormatProperties(physicalDevice, candidate, &formatProperties);

            // Return the candidate if the format has the required fratures
            if((tiling == VK_IMAGE_TILING_LINEAR && (formatProperties.linearTilingFeatures & features) == features) || (tiling == VK_IMAGE_TILING_OPTIMAL && (formatProperties.optimalTilingFeatures & features) == features))
                return candidate;
        }

        // No supported formats were found; throw an error
        console::OutFatalError("Failed to find supported format!", 1);
        return VK_FORMAT_UNDEFINED;
    }

    void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {
        // Set the buffer create info
        VkBufferCreateInfo bufferCreateInfo;

        bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferCreateInfo.pNext = nullptr;
        bufferCreateInfo.flags = 0;
        bufferCreateInfo.size = size;
        bufferCreateInfo.usage = usage;
        bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        bufferCreateInfo.queueFamilyIndexCount = 0;
        bufferCreateInfo.pQueueFamilyIndices = nullptr;

        // Create the buffer
        auto result = vkCreateBuffer(device, &bufferCreateInfo, allocator, &buffer);
        if(result != VK_SUCCESS)
            console::OutFatalError((string)"Failed to create buffer! Error code" + VkResultToString(result), 1);

        // Get the buffer's memory requirements
        VkMemoryRequirements memoryRequirements;
        vkGetBufferMemoryRequirements(device, buffer, &memoryRequirements);

        // Set the memory allocate info
        VkMemoryAllocateInfo memoryAllocateInfo;

        memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        memoryAllocateInfo.pNext = nullptr;
        memoryAllocateInfo.allocationSize = memoryRequirements.size;
        memoryAllocateInfo.memoryTypeIndex = FindMemoryType(memoryRequirements.memoryTypeBits, properties);

        // Allocate the buffer's memory
        result = vkAllocateMemory(device, &memoryAllocateInfo, allocator, &bufferMemory);
        if(result != VK_SUCCESS)
            console::OutFatalError((string)"Failed to allocate buffer memory! Error code: " + VkResultToString(result), 1);
        
        // Bind the buffer's memory
        result = vkBindBufferMemory(device, buffer, bufferMemory, 0);
        if(result != VK_SUCCESS)
            console::OutFatalError((string)"Failed to bind buffer memory! Error code: " + VkResultToString(result), 1);
    }
    void CreateImage(const VkImageCreateInfo& imageInfo, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory) {
        // Create the image
        auto result = vkCreateImage(device, &imageInfo, allocator, &image);
        if(result != VK_SUCCESS)
            console::OutFatalError((string)"Failed to create image! Error code: " + VkResultToString(result), 1);
        
        // Get the image's memory requirements
        VkMemoryRequirements memoryRequirements;
        vkGetImageMemoryRequirements(device, image, &memoryRequirements);

        // Set the memory allocate info
        VkMemoryAllocateInfo memoryAllocateInfo;

        memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        memoryAllocateInfo.pNext = nullptr;
        memoryAllocateInfo.allocationSize = memoryRequirements.size;
        memoryAllocateInfo.memoryTypeIndex = FindMemoryType(memoryRequirements.memoryTypeBits, properties);

        // Allocate the image's memory
        result = vkAllocateMemory(device, &memoryAllocateInfo, allocator, &imageMemory);
        if(result != VK_SUCCESS)
            console::OutFatalError((string)"Failed to allocate image memory! Error code: " + VkResultToString(result), 1);
        
        // Bind the image's memory
        result = vkBindImageMemory(device, image, imageMemory, 0);
        if(result != VK_SUCCESS)
            console::OutFatalError((string)"Failed to bind image memory! Error code: " + VkResultToString(result), 1);
    }
    VkCommandBuffer BeginSingleTimeCommands() {
        // Set the command buffer allocate info
        VkCommandBufferAllocateInfo allocateInfo;

        allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocateInfo.pNext = nullptr;
        allocateInfo.commandPool = commandPool;
        allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocateInfo.commandBufferCount = 1;

        // Allocate the command buffer
        VkCommandBuffer commandBuffer;
        auto result = vkAllocateCommandBuffers(device, &allocateInfo, &commandBuffer);
        if(result != VK_SUCCESS)
            console::OutFatalError((string)"Failed to allocate command buffer! Error code: " + VkResultToString(result), 1);
        
        // Set the begin info
        VkCommandBufferBeginInfo beginInfo;

        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.pNext = nullptr;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        beginInfo.pInheritanceInfo = nullptr;

        // Begin recording the command buffer
        result = vkBeginCommandBuffer(commandBuffer, &beginInfo);
        if(result != VK_SUCCESS)
            console::OutFatalError((string)"Failed to begin recording command buffer! Error code: " + VkResultToString(result), 1);
        
        return commandBuffer;
    }
    void EndSingleTimeCommands(VkCommandBuffer commandBuffer) {
        // End recording the command buffer
        auto result = vkEndCommandBuffer(commandBuffer);
        if(result != VK_SUCCESS)
            console::OutFatalError((string)"Failed to end recording command buffer! Error code: " + VkResultToString(result), 1);

        // Set the command buffer submit info
        VkSubmitInfo submitInfo;

        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.pNext = nullptr;
        submitInfo.waitSemaphoreCount = 0;
        submitInfo.pWaitSemaphores = nullptr;
        submitInfo.pWaitDstStageMask = nullptr;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;
        submitInfo.signalSemaphoreCount = 0;
        submitInfo.pSignalSemaphores = nullptr;

        // Submit the command buffer
        result = vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
        if(result != VK_SUCCESS)
            console::OutFatalError((string)"Failed to submit to queue! Error code: " + VkResultToString(result), 1);
        
        // Wait for the queue to idle
        vkQueueWaitIdle(graphicsQueue);

        // Free the command buffer
        vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
    }
    void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkCommandBuffer commandBuffer) {
        // Begin single time commands if the command buffer's not set
        bool8_t singleTimeCommands = !commandBuffer;
        if(singleTimeCommands)
            commandBuffer = BeginSingleTimeCommands();
        
        // Set the copy region
        VkBufferCopy copyRegion;

        copyRegion.srcOffset = 0;
        copyRegion.dstOffset = 0;
        copyRegion.size = size;

        // Copy the buffer
        vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

        // End single time commands if the command buffer's not set
        if(singleTimeCommands)
            EndSingleTimeCommands(commandBuffer);
    }
    void CopyImage(VkImage srcImage, VkImage dstImage, uint32_t width, uint32_t height, uint32_t srcLayerCount, uint32_t dstLayerCount, VkCommandBuffer commandBuffer) {
        // Begin single time commands if the command buffer's not set
        bool8_t singleTimeCommands = !commandBuffer;
        if(singleTimeCommands)
            commandBuffer = BeginSingleTimeCommands();

        // Set the copy region
        VkImageCopy region;

        region.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.srcSubresource.mipLevel = 0;
        region.srcSubresource.baseArrayLayer = 0;
        region.srcSubresource.layerCount = srcLayerCount;
        region.srcOffset = { 0, 0, 0 };

        region.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.dstSubresource.mipLevel = 0;
        region.dstSubresource.baseArrayLayer = 0;
        region.dstSubresource.layerCount = dstLayerCount;
        region.dstOffset = { 0, 0, 0 };

        region.extent = { width, height, 1 };

        // Copy the image
        vkCmdCopyImage(commandBuffer, srcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

        // End single time commands if the command buffer's not set
        if(singleTimeCommands)
            EndSingleTimeCommands(commandBuffer);
    }
    void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount, VkCommandBuffer commandBuffer) {
        // Begin single time commands if the command buffer's not set
        bool8_t singleTimeCommands = !commandBuffer;
        if(singleTimeCommands)
            commandBuffer = BeginSingleTimeCommands();

        // Set the copy region
        VkBufferImageCopy region;
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;

        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = layerCount;

        region.imageOffset = { 0, 0, 0 };
        region.imageExtent = { width, height, 1 };

        // Copy the buffer to the image
        vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

        // End single time commands if the command buffer's not set
        if(singleTimeCommands)
            EndSingleTimeCommands(commandBuffer);
    }
    void CopyImageToBuffer(VkImage image, VkBuffer buffer, uint32_t width, uint32_t height, uint32_t layerCount, VkCommandBuffer commandBuffer) {
        // Begin single time commands if the command buffer's not set
        bool8_t singleTimeCommands = !commandBuffer;
        if(singleTimeCommands)
            commandBuffer = BeginSingleTimeCommands();
        
        // Set the copy region
        VkBufferImageCopy region;
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;

        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = layerCount;

        region.imageOffset = { 0, 0, 0 };
        region.imageExtent = { width, height, 1 };

        // Copy the image to the buffer
        vkCmdCopyImageToBuffer(commandBuffer, image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, buffer, 1, &region);

        // End single time commands if the command buffer's not set
        if(singleTimeCommands)
            EndSingleTimeCommands(commandBuffer);
    }
    void TransitionImageLayout(VkImage image, VkImageLayout srcLayout, VkImageLayout dstLayout, VkFormat format, VkCommandBuffer commandBuffer) {
        // Begin single time commands if the command buffer's not set
        bool8_t singleTimeCommands = !commandBuffer;
        if(singleTimeCommands)
            commandBuffer = BeginSingleTimeCommands();
        
        VkPipelineStageFlags srcStage, dstStage;

        // Set the image memory barrier
        VkImageMemoryBarrier barrier;

        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.pNext = nullptr;
        
        SetStageAndAccess(srcLayout, barrier.srcAccessMask, srcStage);
        SetStageAndAccess(dstLayout, barrier.dstAccessMask, dstStage);

        barrier.oldLayout = srcLayout;
        barrier.newLayout = dstLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

        barrier.image = image;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;

        // Transition the image layout
        vkCmdPipelineBarrier(commandBuffer, srcStage, dstStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

        // End single time commands if the command buffer's not set
        if(singleTimeCommands)
            EndSingleTimeCommands(commandBuffer);
    }
    VkDeviceSize PadUniformBufferSize(VkDeviceSize originalSize) {
        return (originalSize + physicalDeviceProperties.limits.nonCoherentAtomSize - 1) & ~(physicalDeviceProperties.limits.nonCoherentAtomSize - 1);
    }
}