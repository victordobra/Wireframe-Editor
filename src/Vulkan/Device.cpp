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
        QueueFamilyIndices indices;

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
        console::OutMessageFunction("Create Vulkan window surface successfully.");
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
        console::OutMessageFunction((string)"Found suitable GPU: " + physicalDeviceProperties.deviceName);
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
        VkPhysicalDeviceFeatures physicalDeviceFeatues{};

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
}