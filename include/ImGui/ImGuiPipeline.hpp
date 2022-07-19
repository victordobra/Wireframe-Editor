#pragma once

#include "Vulkan/VulkanInclude.hpp"

namespace wfe::editor {
    void CreateImGuiPipeline();
    void DeleteImGuiPipeline();

    void RenderImGui();

    VkPipeline GetPipeline();
    VkPipelineLayout GetPipelineLayout();
    VkDescriptorPool GetDescriptorPool();
}