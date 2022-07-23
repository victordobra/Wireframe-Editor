#pragma once

#include "VulkanInclude.hpp"

namespace wfe::editor {
    struct QueueFamilyIndices {
        uint32_t graphicsFamily; bool8_t graphicsFamilyHasValue;
        uint32_t presentFamily;  bool8_t presentFamilyHasValue;

        bool8_t IsComplete() {
            return graphicsFamilyHasValue && presentFamilyHasValue;
        }
    };
    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        vector<VkSurfaceFormatKHR> formats;
        vector<VkPresentModeKHR> presentModes;
    };

#ifdef NDEBUG
    const bool8_t enableValidationLayers = false;
#else
    const bool8_t enableValidationLayers = true;
#endif

    void CreateDevice();
    void DeleteDevice();
}