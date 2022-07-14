#pragma once

#include "Vulkan/VulkanInclude.hpp"
#include "Core.hpp"

namespace wfe::editor {
    void CreateGUIPipeline();
    void DeleteGUIPipeline();

    VkPipeline GetGUIPipeline();
    VkPipelineLayout GetGUIPipelineLayout();

    void Draw();
}