#pragma once

#ifndef WFE_VULKAN_UTILS_DONT_INCLUDE_VULKAN
#include "VulkanInclude.hpp"
#endif
#include "Core.hpp"

namespace wfe::editor {
    /// @brief Converts the given VkResult to a string
    string VkResultToString(VkResult result);
}