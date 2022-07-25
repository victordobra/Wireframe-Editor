#include "Vulkan/ImGuiPipeline.hpp"
#include "Vulkan/Device.hpp"
#include "Vulkan/SwapChain.hpp"
#include "imgui.hpp"

namespace wfe::editor {
    struct PushConstants {
        float32_t scale[2];
    };

    // Constants

    /*
    #version 450
    
    layout(location = 0) in vec2 position;
    layout(location = 1) in vec2 uvCoord;
    layout(location = 2) in vec4 color;
    
    layout(location = 0) out vec2 fragUvCoord;
    layout(location = 1) out vec4 fragColor;
    
    layout(push_constant) uniform Push {
        vec2 scale;
    } push;
    
    void main() {
        gl_Position = vec4(position * push.scale - 1.0, 0.0, 1.0);
        fragUvCoord = uvCoord;
        fragColor = color;
    }
    */
    const uint32_t vertShaderCode[] = {
        0x07230203,0x00010000,0x0008000a,0x0000002c,0x00000000,0x00020011,0x00000001,0x0006000b,
        0x00000001,0x4c534c47,0x6474732e,0x3035342e,0x00000000,0x0003000e,0x00000000,0x00000001,
        0x000b000f,0x00000000,0x00000004,0x6e69616d,0x00000000,0x0000000d,0x00000012,0x00000025,
        0x00000026,0x00000028,0x0000002a,0x00030003,0x00000002,0x000001c2,0x00040005,0x00000004,
        0x6e69616d,0x00000000,0x00060005,0x0000000b,0x505f6c67,0x65567265,0x78657472,0x00000000,
        0x00060006,0x0000000b,0x00000000,0x505f6c67,0x7469736f,0x006e6f69,0x00070006,0x0000000b,
        0x00000001,0x505f6c67,0x746e696f,0x657a6953,0x00000000,0x00070006,0x0000000b,0x00000002,
        0x435f6c67,0x4470696c,0x61747369,0x0065636e,0x00070006,0x0000000b,0x00000003,0x435f6c67,
        0x446c6c75,0x61747369,0x0065636e,0x00030005,0x0000000d,0x00000000,0x00050005,0x00000012,
        0x69736f70,0x6e6f6974,0x00000000,0x00040005,0x00000014,0x68737550,0x00000000,0x00050006,
        0x00000014,0x00000000,0x6c616373,0x00000065,0x00040005,0x00000016,0x68737570,0x00000000,
        0x00050005,0x00000025,0x67617266,0x6f437655,0x0064726f,0x00040005,0x00000026,0x6f437675,
        0x0064726f,0x00050005,0x00000028,0x67617266,0x6f6c6f43,0x00000072,0x00040005,0x0000002a,
        0x6f6c6f63,0x00000072,0x00050048,0x0000000b,0x00000000,0x0000000b,0x00000000,0x00050048,
        0x0000000b,0x00000001,0x0000000b,0x00000001,0x00050048,0x0000000b,0x00000002,0x0000000b,
        0x00000003,0x00050048,0x0000000b,0x00000003,0x0000000b,0x00000004,0x00030047,0x0000000b,
        0x00000002,0x00040047,0x00000012,0x0000001e,0x00000000,0x00050048,0x00000014,0x00000000,
        0x00000023,0x00000000,0x00030047,0x00000014,0x00000002,0x00040047,0x00000025,0x0000001e,
        0x00000000,0x00040047,0x00000026,0x0000001e,0x00000001,0x00040047,0x00000028,0x0000001e,
        0x00000001,0x00040047,0x0000002a,0x0000001e,0x00000002,0x00020013,0x00000002,0x00030021,
        0x00000003,0x00000002,0x00030016,0x00000006,0x00000020,0x00040017,0x00000007,0x00000006,
        0x00000004,0x00040015,0x00000008,0x00000020,0x00000000,0x0004002b,0x00000008,0x00000009,
        0x00000001,0x0004001c,0x0000000a,0x00000006,0x00000009,0x0006001e,0x0000000b,0x00000007,
        0x00000006,0x0000000a,0x0000000a,0x00040020,0x0000000c,0x00000003,0x0000000b,0x0004003b,
        0x0000000c,0x0000000d,0x00000003,0x00040015,0x0000000e,0x00000020,0x00000001,0x0004002b,
        0x0000000e,0x0000000f,0x00000000,0x00040017,0x00000010,0x00000006,0x00000002,0x00040020,
        0x00000011,0x00000001,0x00000010,0x0004003b,0x00000011,0x00000012,0x00000001,0x0003001e,
        0x00000014,0x00000010,0x00040020,0x00000015,0x00000009,0x00000014,0x0004003b,0x00000015,
        0x00000016,0x00000009,0x00040020,0x00000017,0x00000009,0x00000010,0x0004002b,0x00000006,
        0x0000001b,0x3f800000,0x0004002b,0x00000006,0x0000001e,0x00000000,0x00040020,0x00000022,
        0x00000003,0x00000007,0x00040020,0x00000024,0x00000003,0x00000010,0x0004003b,0x00000024,
        0x00000025,0x00000003,0x0004003b,0x00000011,0x00000026,0x00000001,0x0004003b,0x00000022,
        0x00000028,0x00000003,0x00040020,0x00000029,0x00000001,0x00000007,0x0004003b,0x00000029,
        0x0000002a,0x00000001,0x00050036,0x00000002,0x00000004,0x00000000,0x00000003,0x000200f8,
        0x00000005,0x0004003d,0x00000010,0x00000013,0x00000012,0x00050041,0x00000017,0x00000018,
        0x00000016,0x0000000f,0x0004003d,0x00000010,0x00000019,0x00000018,0x00050085,0x00000010,
        0x0000001a,0x00000013,0x00000019,0x00050050,0x00000010,0x0000001c,0x0000001b,0x0000001b,
        0x00050083,0x00000010,0x0000001d,0x0000001a,0x0000001c,0x00050051,0x00000006,0x0000001f,
        0x0000001d,0x00000000,0x00050051,0x00000006,0x00000020,0x0000001d,0x00000001,0x00070050,
        0x00000007,0x00000021,0x0000001f,0x00000020,0x0000001e,0x0000001b,0x00050041,0x00000022,
        0x00000023,0x0000000d,0x0000000f,0x0003003e,0x00000023,0x00000021,0x0004003d,0x00000010,
        0x00000027,0x00000026,0x0003003e,0x00000025,0x00000027,0x0004003d,0x00000007,0x0000002b,
        0x0000002a,0x0003003e,0x00000028,0x0000002b,0x000100fd,0x00010038
    };
    /*
    #version 450

    layout(location = 0) in vec2 fragUvCoord;
    layout(location = 1) in vec4 fragColor;

    layout(location = 0) out vec4 outColor;

    layout(set = 0, binding = 0) uniform sampler2D fontTexture;

    void main() {
        outColor = fragColor * texture(fontTexture, fragUvCoord);
    }
    */
    const uint32_t fragShaderCode[] = {
        0x07230203,0x00010000,0x0008000a,0x00000018,0x00000000,0x00020011,0x00000001,0x0006000b,
        0x00000001,0x4c534c47,0x6474732e,0x3035342e,0x00000000,0x0003000e,0x00000000,0x00000001,
        0x0008000f,0x00000004,0x00000004,0x6e69616d,0x00000000,0x00000009,0x0000000b,0x00000014,
        0x00030010,0x00000004,0x00000007,0x00030003,0x00000002,0x000001c2,0x00040005,0x00000004,
        0x6e69616d,0x00000000,0x00050005,0x00000009,0x4374756f,0x726f6c6f,0x00000000,0x00050005,
        0x0000000b,0x67617266,0x6f6c6f43,0x00000072,0x00050005,0x00000010,0x746e6f66,0x74786554,
        0x00657275,0x00050005,0x00000014,0x67617266,0x6f437655,0x0064726f,0x00040047,0x00000009,
        0x0000001e,0x00000000,0x00040047,0x0000000b,0x0000001e,0x00000001,0x00040047,0x00000010,
        0x00000022,0x00000000,0x00040047,0x00000010,0x00000021,0x00000000,0x00040047,0x00000014,
        0x0000001e,0x00000000,0x00020013,0x00000002,0x00030021,0x00000003,0x00000002,0x00030016,
        0x00000006,0x00000020,0x00040017,0x00000007,0x00000006,0x00000004,0x00040020,0x00000008,
        0x00000003,0x00000007,0x0004003b,0x00000008,0x00000009,0x00000003,0x00040020,0x0000000a,
        0x00000001,0x00000007,0x0004003b,0x0000000a,0x0000000b,0x00000001,0x00090019,0x0000000d,
        0x00000006,0x00000001,0x00000000,0x00000000,0x00000000,0x00000001,0x00000000,0x0003001b,
        0x0000000e,0x0000000d,0x00040020,0x0000000f,0x00000000,0x0000000e,0x0004003b,0x0000000f,
        0x00000010,0x00000000,0x00040017,0x00000012,0x00000006,0x00000002,0x00040020,0x00000013,
        0x00000001,0x00000012,0x0004003b,0x00000013,0x00000014,0x00000001,0x00050036,0x00000002,
        0x00000004,0x00000000,0x00000003,0x000200f8,0x00000005,0x0004003d,0x00000007,0x0000000c,
        0x0000000b,0x0004003d,0x0000000e,0x00000011,0x00000010,0x0004003d,0x00000012,0x00000015,
        0x00000014,0x00050057,0x00000007,0x00000016,0x00000011,0x00000015,0x00050085,0x00000007,
        0x00000017,0x0000000c,0x00000016,0x0003003e,0x00000009,0x00000017,0x000100fd,0x00010038
    };

    // Variables
    VkPipeline pipeline;
    VkPipelineLayout layout;
    VkShaderModule vertShaderModule, fragShaderModule;

    VkSampler sampler;

    VkDescriptorPool descriptorPool;
    VkDescriptorSetLayout fontDescriptorSetLayout;
    VkDescriptorSet fontDescriptorSets[MAX_FRAMES_IN_FLIGHT];
    VkImage fontImages[MAX_FRAMES_IN_FLIGHT];
    VkDeviceMemory fontImageMemories[MAX_FRAMES_IN_FLIGHT];
    VkImageView fontImageViews[MAX_FRAMES_IN_FLIGHT];

    static void ConfigureImGuiBackend() {
        ImGui::GetIO().BackendRendererName = "WFE-Vulkan-Renderer";
        ImGui::GetIO().BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;
    }
    static void CreateSampler() {
        // Set the sampler create info
        VkSamplerCreateInfo createInfo;

        createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.magFilter = VK_FILTER_LINEAR;
        createInfo.minFilter = VK_FILTER_LINEAR;
        createInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        createInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        createInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        createInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        createInfo.mipLodBias = 0.f;
        createInfo.anisotropyEnable = VK_FALSE;
        createInfo.maxAnisotropy = 0.f;
        createInfo.compareEnable = VK_FALSE;
        createInfo.compareOp = VK_COMPARE_OP_NEVER;
        createInfo.minLod = -1000.f;
        createInfo.maxLod = 1000.f;
        createInfo.borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
        createInfo.unnormalizedCoordinates = VK_FALSE;

        // Create the sampler
        auto result = vkCreateSampler(GetDevice(), &createInfo, GetVulkanAllocator(), &sampler);
        if(result != VK_SUCCESS)
            console::OutFatalError((string)"Failed to create sampler! Error code: " + VkResultToString(result), 1);
    }
    static void CreateDescriptorPool() {
        // Set the descriptor pool size
        VkDescriptorPoolSize poolSize = { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, MAX_FRAMES_IN_FLIGHT };

        // Set the descriptor pool create info
        VkDescriptorPoolCreateInfo createInfo;

        createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.maxSets = MAX_FRAMES_IN_FLIGHT;
        createInfo.poolSizeCount = 1;
        createInfo.pPoolSizes = &poolSize;

        // Create the descriptor pool
        auto result = vkCreateDescriptorPool(GetDevice(), &createInfo, GetVulkanAllocator(), &descriptorPool);
        if(result != VK_SUCCESS)
            console::OutFatalError((string)"Failed to create descriptor pool! Error code: " + VkResultToString(result), 1);
    }
    static void CreateFontDescriptorSets() {
        // Set the descriptor set layout binding
        VkDescriptorSetLayoutBinding binding;
        binding.binding = 0;
        binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        binding.descriptorCount = 1;
        binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        binding.pImmutableSamplers = nullptr;

        // Set the descriptor set layout create info
        VkDescriptorSetLayoutCreateInfo createInfo;

        createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.bindingCount = 1;
        createInfo.pBindings = &binding;

        // Create the descriptor set layout
        auto result = vkCreateDescriptorSetLayout(GetDevice(), &createInfo, GetVulkanAllocator(), &fontDescriptorSetLayout);
        if(result != VK_SUCCESS)
            console::OutFatalError((string)"Failed to create font descriptor set layout! Error code: " + VkResultToString(result), 1);

        // Create the array of descriptor set layouts
        VkDescriptorSetLayout layouts[MAX_FRAMES_IN_FLIGHT];
        for(size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
            layouts[i] = fontDescriptorSetLayout;

        // Set the descriptor set alloc info
        VkDescriptorSetAllocateInfo allocInfo;

        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.pNext = nullptr;
        allocInfo.descriptorPool = descriptorPool;
        allocInfo.descriptorSetCount = 2;
        allocInfo.pSetLayouts = layouts;

        // Allocate the descriptor sets
        result = vkAllocateDescriptorSets(GetDevice(), &allocInfo, fontDescriptorSets);
        if(result != VK_SUCCESS)
            console::OutFatalError((string)"Failed to allocate descriptor sets! Error code: " + VkResultToString(result), 1);
    }
    static void CreateFontImages() {
        // Get the font texture data
        int32_t width, height, pixelSize;
        uint8_t* data;
        ImGui::GetIO().Fonts->GetTexDataAsRGBA32(&data, &width, &height, &pixelSize);

        // Create the staging buffer
        VkDeviceSize imageSize = (VkDeviceSize)(width * height * pixelSize);

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;

        CreateBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, stagingBuffer, stagingBufferMemory);

        // Write the the staging buffer
        void* mappedMemory;

        vkMapMemory(GetDevice(), stagingBufferMemory, 0, imageSize, 0, &mappedMemory);

        memcpy(mappedMemory, data, (size_t)imageSize);

        VkMappedMemoryRange memoryRange;

        memoryRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        memoryRange.pNext = nullptr;
        memoryRange.memory = stagingBufferMemory;
        memoryRange.offset = 0;
        memoryRange.size = imageSize;

        vkFlushMappedMemoryRanges(GetDevice(), 1, &memoryRange);

        vkUnmapMemory(GetDevice(), stagingBufferMemory);

        // Set the image create info
        VkImageCreateInfo imageInfo;

        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.pNext = nullptr;
        imageInfo.flags = 0;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
        imageInfo.extent = { (uint32_t)width, (uint32_t)height, 1 };
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.queueFamilyIndexCount = 0;
        imageInfo.pQueueFamilyIndices = nullptr;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

        // Set the image view create info without the image; it will be set in the for loop
        VkImageViewCreateInfo imageViewInfo;

        imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewInfo.pNext = nullptr;
        imageViewInfo.flags = 0;
        imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageViewInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
        imageViewInfo.components = { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY };
        
        imageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageViewInfo.subresourceRange.baseMipLevel = 0;
        imageViewInfo.subresourceRange.levelCount = 1;
        imageViewInfo.subresourceRange.baseArrayLayer = 0;
        imageViewInfo.subresourceRange.layerCount = 1;

        // Begin single time commands
        VkCommandBuffer commandBuffer = BeginSingleTimeCommands();

        // Create every image
        for(size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
            // Create the image
            CreateImage(imageInfo, VK_MEMORY_HEAP_DEVICE_LOCAL_BIT, fontImages[i], fontImageMemories[i]);

            // Create the image view
            imageViewInfo.image = fontImages[i];

            auto result = vkCreateImageView(GetDevice(), &imageViewInfo, GetVulkanAllocator(), fontImageViews + i);
            if(result != VK_SUCCESS)
                console::OutFatalError((string)"Failed to create image view! Error code: " + VkResultToString(result), 1);
            
            // Copy to the image
            TransitionImageLayout(fontImages[i], VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_FORMAT_R8G8B8A8_UNORM, commandBuffer);
            CopyBufferToImage(stagingBuffer, fontImages[i], (uint32_t)width, (uint32_t)height, 1, commandBuffer);
            TransitionImageLayout(fontImages[i], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_FORMAT_R8G8B8A8_UNORM, commandBuffer);
        }

        // End single time commands
        EndSingleTimeCommands(commandBuffer);

        // Set the descriptor image infos
        VkDescriptorImageInfo descriptorImageInfos[MAX_FRAMES_IN_FLIGHT];

        for(size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
            descriptorImageInfos[i].sampler = sampler;
            descriptorImageInfos[i].imageView = fontImageViews[i];
            descriptorImageInfos[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        }

        // Set the descriptor set write infos
        VkWriteDescriptorSet writes[MAX_FRAMES_IN_FLIGHT];

        for(size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
            writes[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writes[i].pNext = nullptr;
            writes[i].dstSet = fontDescriptorSets[i];
            writes[i].dstBinding = 0;
            writes[i].dstArrayElement = 0;
            writes[i].descriptorCount = 1;
            writes[i].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            writes[i].pImageInfo = descriptorImageInfos + i;
            writes[i].pBufferInfo = nullptr;
            writes[i].pTexelBufferView = nullptr;
        }
        
        // Update the descriptor sets
        vkUpdateDescriptorSets(GetDevice(), MAX_FRAMES_IN_FLIGHT, writes, 0, nullptr);

        // Delete the staging buffer
        vkDestroyBuffer(GetDevice(), stagingBuffer, GetVulkanAllocator());
        vkFreeMemory(GetDevice(), stagingBufferMemory, GetVulkanAllocator());
    }
    static void CreatePipelineLayout() {
        // Set the push constant range
        VkPushConstantRange pushConstantRange;

        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(PushConstants);

        // Set the pipeline layout create info
        VkPipelineLayoutCreateInfo createInfo;
        
        createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.setLayoutCount = 1;
        createInfo.pSetLayouts = &fontDescriptorSetLayout;
        createInfo.pushConstantRangeCount = 1;
        createInfo.pPushConstantRanges = &pushConstantRange;

        auto result = vkCreatePipelineLayout(GetDevice(), &createInfo, GetVulkanAllocator(), &layout);
        if(result != VK_SUCCESS)
            console::OutFatalError((string)"Failed to create pipeline layout! Error code: " + VkResultToString(result), 1);
    }
    static void CreateShaderModules() {
        // Set the vertex shader module create info
        VkShaderModuleCreateInfo vertShaderInfo;

        vertShaderInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        vertShaderInfo.pNext = nullptr;
        vertShaderInfo.flags = 0;
        vertShaderInfo.codeSize = sizeof(vertShaderCode);
        vertShaderInfo.pCode = vertShaderCode;

        // Set the fragment shader module create info
        VkShaderModuleCreateInfo fragShaderInfo;

        fragShaderInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        fragShaderInfo.pNext = nullptr;
        fragShaderInfo.flags = 0;
        fragShaderInfo.codeSize = sizeof(fragShaderCode);
        fragShaderInfo.pCode = fragShaderCode;

        // Create the vertex shader module
        auto result = vkCreateShaderModule(GetDevice(), &vertShaderInfo, GetVulkanAllocator(), &vertShaderModule);
        if(result != VK_SUCCESS)
            console::OutFatalError((string)"Failed to create vertex shader module! Error code: " + VkResultToString(result), 1);
        
        // Create the fragment shader module
        result = vkCreateShaderModule(GetDevice(), &fragShaderInfo, GetVulkanAllocator(), &fragShaderModule);
        if(result != VK_SUCCESS)
            console::OutFatalError((string)"Failed to create fragment shader module! Error code: " + VkResultToString(result), 1);
    }
    static void CreateGraphicsPipeline() {
        // Set the vertex binding description
        VkVertexInputBindingDescription bindingDescription;
        
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(ImDrawVert);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        // Set the vertex attribute descriptions
        VkVertexInputAttributeDescription attributeDescriptions[3];

        attributeDescriptions[0] = { 0, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(ImDrawVert, pos) };
        attributeDescriptions[1] = { 1, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(ImDrawVert, uv) };
        attributeDescriptions[2] = { 2, 0, VK_FORMAT_R8G8B8A8_UNORM, offsetof(ImDrawVert, col) };

        // Set the vertex input info
        VkPipelineVertexInputStateCreateInfo vertexInputInfo;

        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.pNext = nullptr;
        vertexInputInfo.flags = 0;
        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
        vertexInputInfo.vertexAttributeDescriptionCount = 3;
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions;

        // Set the input assembly info
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;

        inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssemblyInfo.pNext = nullptr;
        inputAssemblyInfo.flags = 0;
        inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

        // Set the viewport info
        VkPipelineViewportStateCreateInfo viewportInfo;
        
        viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportInfo.pNext = nullptr;
        viewportInfo.flags = 0;
        viewportInfo.viewportCount = 1;
        viewportInfo.pViewports = nullptr;
        viewportInfo.scissorCount = 1;
        viewportInfo.pScissors = nullptr;

        // Set the rasterization info
        VkPipelineRasterizationStateCreateInfo rasterizationInfo;

        rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizationInfo.pNext = nullptr;
        rasterizationInfo.flags = 0;
        rasterizationInfo.depthClampEnable = VK_FALSE;
        rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
        rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizationInfo.cullMode = VK_CULL_MODE_NONE;
        rasterizationInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterizationInfo.depthBiasEnable = VK_FALSE;
        rasterizationInfo.depthBiasConstantFactor = 0.f;
        rasterizationInfo.depthBiasClamp = 0.f;
        rasterizationInfo.depthBiasSlopeFactor = 0.f;
        rasterizationInfo.lineWidth = 1.f;

        // Set the multisample info
        VkPipelineMultisampleStateCreateInfo multisampleInfo;

        multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampleInfo.pNext = nullptr;
        multisampleInfo.flags = 0;
        multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisampleInfo.sampleShadingEnable = VK_FALSE;
        multisampleInfo.minSampleShading = 0.f;
        multisampleInfo.pSampleMask = nullptr;
        multisampleInfo.alphaToCoverageEnable = VK_FALSE;
        multisampleInfo.alphaToOneEnable = VK_FALSE;

        // Set the depth stencil info
        VkPipelineDepthStencilStateCreateInfo depthStencilInfo;

        depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencilInfo.pNext = nullptr;
        depthStencilInfo.flags = 0;
        depthStencilInfo.depthTestEnable = VK_FALSE;
        depthStencilInfo.depthWriteEnable = VK_FALSE;
        depthStencilInfo.depthCompareOp = VK_COMPARE_OP_NEVER;
        depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
        depthStencilInfo.stencilTestEnable = VK_FALSE;

        depthStencilInfo.front.failOp = VK_STENCIL_OP_KEEP;
        depthStencilInfo.front.passOp = VK_STENCIL_OP_KEEP;
        depthStencilInfo.front.depthFailOp = VK_STENCIL_OP_KEEP;
        depthStencilInfo.front.compareOp = VK_COMPARE_OP_NEVER;
        depthStencilInfo.front.compareMask = 0;
        depthStencilInfo.front.writeMask = 0;
        depthStencilInfo.front.reference = 0;

        depthStencilInfo.back.failOp = VK_STENCIL_OP_KEEP;
        depthStencilInfo.back.passOp = VK_STENCIL_OP_KEEP;
        depthStencilInfo.back.depthFailOp = VK_STENCIL_OP_KEEP;
        depthStencilInfo.back.compareOp = VK_COMPARE_OP_NEVER;
        depthStencilInfo.back.compareMask = 0;
        depthStencilInfo.back.writeMask = 0;
        depthStencilInfo.back.reference = 0;

        depthStencilInfo.minDepthBounds = 0.f;
        depthStencilInfo.maxDepthBounds = 0.f;

        // Set the color attachment
        VkPipelineColorBlendAttachmentState colorAttachment;

        colorAttachment.blendEnable = VK_TRUE;
        colorAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        colorAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        colorAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        colorAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        colorAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        colorAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
        colorAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

        // Set the color blend info
        VkPipelineColorBlendStateCreateInfo colorBlendInfo;
        
        colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlendInfo.pNext = nullptr;
        colorBlendInfo.flags = 0;
        colorBlendInfo.logicOpEnable = VK_FALSE;
        colorBlendInfo.logicOp = VK_LOGIC_OP_CLEAR;
        colorBlendInfo.attachmentCount = 1;
        colorBlendInfo.pAttachments = &colorAttachment;
        colorBlendInfo.blendConstants[0] = 0.f;
        colorBlendInfo.blendConstants[1] = 0.f;
        colorBlendInfo.blendConstants[2] = 0.f;
        colorBlendInfo.blendConstants[3] = 0.f;

        // Set the dynamic states
        VkDynamicState dynamicStates[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

        // Set the dynamic state info
        VkPipelineDynamicStateCreateInfo dynamicStateInfo;

        dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicStateInfo.pNext = nullptr;
        dynamicStateInfo.flags = 0;
        dynamicStateInfo.dynamicStateCount = 2;
        dynamicStateInfo.pDynamicStates = dynamicStates;

        // Set the shader stages
        VkPipelineShaderStageCreateInfo shaderStages[2];

        shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[0].pNext = nullptr;
        shaderStages[0].flags = 0;
        shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        shaderStages[0].module = vertShaderModule;
        shaderStages[0].pName = "main";
        shaderStages[0].pSpecializationInfo = nullptr;

        shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[1].pNext = nullptr;
        shaderStages[1].flags = 0;
        shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        shaderStages[1].module = fragShaderModule;
        shaderStages[1].pName = "main";
        shaderStages[1].pSpecializationInfo = nullptr;

        // Set the graphics pipeline create info
        VkGraphicsPipelineCreateInfo createInfo;

        createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.stageCount = 2;
        createInfo.pStages = shaderStages;
        createInfo.pVertexInputState = &vertexInputInfo;
        createInfo.pInputAssemblyState = &inputAssemblyInfo;
        createInfo.pTessellationState = nullptr;
        createInfo.pViewportState = &viewportInfo;
        createInfo.pRasterizationState = &rasterizationInfo;
        createInfo.pMultisampleState = &multisampleInfo;
        createInfo.pDepthStencilState = &depthStencilInfo;
        createInfo.pColorBlendState = &colorBlendInfo;
        createInfo.pDynamicState = &dynamicStateInfo;
        createInfo.layout = layout;
        createInfo.renderPass = GetRenderPass();
        createInfo.subpass = 0;
        createInfo.basePipelineHandle = VK_NULL_HANDLE;
        createInfo.basePipelineIndex = -1;

        // Create the graphics pipeline
        auto result = vkCreateGraphicsPipelines(GetDevice(), VK_NULL_HANDLE, 1, &createInfo, GetVulkanAllocator(), &pipeline);
        if(result != VK_SUCCESS)
            console::OutFatalError((string)"Failed to create graphics pipeline! Error code: " + VkResultToString(result), 1);
    }

    // Public functions
    void CreateImGuiPipeline() {
        CreateSampler();
        CreateDescriptorPool();
        CreateFontDescriptorSets();
        CreateFontImages();
        CreatePipelineLayout();
        CreateShaderModules();
        CreateGraphicsPipeline();

        console::OutMessageFunction("Created ImGui graphics pipeline successfully.");
    }
    void DeleteImGuiPipeline() {
        vkDeviceWaitIdle(GetDevice());
        vkDestroyPipeline(GetDevice(), pipeline, GetVulkanAllocator());
        vkDestroyShaderModule(GetDevice(), vertShaderModule, GetVulkanAllocator());
        vkDestroyShaderModule(GetDevice(), fragShaderModule, GetVulkanAllocator());
        vkDestroyPipelineLayout(GetDevice(), layout, GetVulkanAllocator());

        for(size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
            vkDestroyImage(GetDevice(), fontImages[i], GetVulkanAllocator());
            vkFreeMemory(GetDevice(), fontImageMemories[i], GetVulkanAllocator());
            vkDestroyImageView(GetDevice(), fontImageViews[i], GetVulkanAllocator());
        } 

        vkDestroyDescriptorSetLayout(GetDevice(), fontDescriptorSetLayout, GetVulkanAllocator());
        vkDestroyDescriptorPool(GetDevice(), descriptorPool, GetVulkanAllocator());
        vkDestroySampler(GetDevice(), sampler, GetVulkanAllocator());

        console::OutMessageFunction("Deleted ImGui graphics pipeline successfully.");
    }
    void DrawImGui() {
        // Acquire the next swap chain image
        uint32_t imageIndex;
        auto result = AcquireNextImage(&imageIndex);
        if(result == VK_ERROR_OUT_OF_DATE_KHR)
            return;
        if(result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
            console::OutFatalError((string)"Failed to acquire next swap chain image! Error code: " + VkResultToString(result), 1);

        // Get the ImGui draw data
        ImDrawData* drawData = ImGui::GetDrawData();

        VkBuffer vertexBuffer, indexBuffer;
        VkDeviceMemory vertexBufferMemory, indexBufferMemory;

        if(drawData->TotalVtxCount) {
            // Create the vertex and index buffers
            VkDeviceSize vertexBufferSize = drawData->TotalVtxCount * sizeof(ImDrawVert);
            VkDeviceSize indexBufferSize = drawData->TotalIdxCount * sizeof(ImDrawIdx);

            CreateBuffer(vertexBufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, vertexBuffer, vertexBufferMemory);
            CreateBuffer(indexBufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, indexBuffer, indexBufferMemory);

            // Write to the vertex and index buffers
            ImDrawVert *dstVert;
            ImDrawIdx *dstInd;

            vkMapMemory(GetDevice(), vertexBufferMemory, 0, vertexBufferSize, 0, (void**)&dstVert);
            vkMapMemory(GetDevice(), indexBufferMemory, 0, indexBufferSize, 0, (void**)&dstInd);

            for(size_t i = 0; i < drawData->CmdListsCount; ++i) {
                memcpy(dstVert, drawData->CmdLists[i]->VtxBuffer.Data, drawData->CmdLists[i]->VtxBuffer.Size * sizeof(ImDrawVert));
                memcpy(dstInd, drawData->CmdLists[i]->IdxBuffer.Data, drawData->CmdLists[i]->IdxBuffer.Size * sizeof(ImDrawIdx));

                dstVert += drawData->CmdLists[i]->VtxBuffer.Size;
                dstInd += drawData->CmdLists[i]->IdxBuffer.Size;
            }

            // Flush the changes and unmap the buffers
            VkMappedMemoryRange ranges[2];

            ranges[0].sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
            ranges[0].pNext = nullptr;
            ranges[0].memory = vertexBufferMemory;
            ranges[0].offset = 0;
            ranges[0].size = vertexBufferSize;

            ranges[1].sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
            ranges[1].pNext = nullptr;
            ranges[1].memory = indexBufferMemory;
            ranges[1].offset = 0;
            ranges[1].size = indexBufferSize;

            vkFlushMappedMemoryRanges(GetDevice(), 2, ranges);

            vkUnmapMemory(GetDevice(), vertexBufferMemory);
            vkUnmapMemory(GetDevice(), indexBufferMemory);
        }

        VkCommandBuffer commandBuffer;

        // Allocate the command buffers
        VkCommandBufferAllocateInfo allocInfo;

        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.pNext = nullptr;
        allocInfo.commandPool = GetCommandPool();
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = 1;

        result = vkAllocateCommandBuffers(GetDevice(), &allocInfo, &commandBuffer);
        if(result != VK_SUCCESS)
            console::OutFatalError((string)"Failed to allocate command buffers! Error code: " + VkResultToString(result), 1);
        
        // Begin recording the command buffer
        VkCommandBufferBeginInfo beginInfo;

        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.pNext = nullptr;
        beginInfo.flags = 0;
        beginInfo.pInheritanceInfo = nullptr;

        result = vkBeginCommandBuffer(commandBuffer, &beginInfo);
        if(result != VK_SUCCESS)
            console::OutFatalError((string)"Failed to begin recording command buffer! Error code: " + VkResultToString(result), 1);

        // Set the clear values
        VkClearValue clearValues[2];
        clearValues[0] = { 0.f, 0.f, 0.f, 1.f };
        clearValues[1] = { 1.f, 0 };

        // Begin the render pass
        VkRenderPassBeginInfo renderPassInfo;

        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.pNext = nullptr;
        renderPassInfo.renderPass = GetRenderPass();
        renderPassInfo.framebuffer = GetFrameBuffer(imageIndex);
        renderPassInfo.renderArea = { { 0, 0 }, GetSwapChainExtent() };
        renderPassInfo.clearValueCount = 2;
        renderPassInfo.pClearValues = clearValues;

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        // Create the viewport
        VkViewport viewport = { 0.f, 0.f, (float32_t)GetSwapChainWidth(), (float32_t)GetSwapChainHeight(), 0.f, 1.f };
        
        // Set the push constants
        PushConstants pushConstants;
        pushConstants.scale[0] = 2.f / GetSwapChainWidth();
        pushConstants.scale[1] = 2.f / GetSwapChainHeight();

        vkCmdPushConstants(commandBuffer, layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConstants), &pushConstants);

        // Bind the font descriptor set
        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, layout, 0, 1, fontDescriptorSets + GetCurrentFrame(), 0, nullptr);

        VkDeviceSize vertOffset = 0, indOffset = 0;
        for(size_t i = 0; i < drawData->CmdListsCount; ++i) {
            ImDrawList* cmdList = drawData->CmdLists[i];
            for(size_t j = 0; j < cmdList->CmdBuffer.Size; ++j) {
                ImDrawCmd* drawCmd = &cmdList->CmdBuffer[j];
                
                if(drawCmd->UserCallback) {
                    // Run the specified user callback
                    if(drawCmd->UserCallback != ImDrawCallback_ResetRenderState)
                        drawCmd->UserCallback(cmdList, drawCmd);
                } else {
                    // Bind the pipeline
                    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

                    // Set the viewport
                    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

                    // Set the scissor
                    VkRect2D scissor = { { (int32_t)drawCmd->ClipRect.x, (int32_t)drawCmd->ClipRect.y }, { (uint32_t)drawCmd->ClipRect.z - (uint32_t)drawCmd->ClipRect.x, (uint32_t)drawCmd->ClipRect.w - (uint32_t)drawCmd->ClipRect.y } };
                    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

                    // Push constants
                    vkCmdPushConstants(commandBuffer, layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConstants), &pushConstants);

                    // Bind the font descriptor set
                    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, layout, 0, 1, fontDescriptorSets + GetCurrentFrame(), 0, nullptr);

                    // Bind the vertex and index buffers
                    VkDeviceSize offset = 0;
                    vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertexBuffer, &offset);
                    vkCmdBindIndexBuffer(commandBuffer, indexBuffer, offset, VK_INDEX_TYPE_UINT16);

                    // Draw the UI elements
                    vkCmdDrawIndexed(commandBuffer, drawCmd->ElemCount, 1, drawCmd->IdxOffset + indOffset, drawCmd->VtxOffset + vertOffset, 0);
                }
            }

            vertOffset += cmdList->VtxBuffer.Size;
            indOffset += cmdList->IdxBuffer.Size;
        }

        // End the render pass
        vkCmdEndRenderPass(commandBuffer);

        // End recording the command buffer
        result = vkEndCommandBuffer(commandBuffer);
        if(result != VK_SUCCESS)
            console::OutFatalError((string)"Failed to end recording command buffer! Error code: " + VkResultToString(result), 1);
        
        // Submit the command buffer
        result = SubmitCommandBuffers(&commandBuffer, &imageIndex);
        if(result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR && result != VK_ERROR_OUT_OF_DATE_KHR)
            console::OutFatalError((string)"Failed to submit command buffer! Error code: " + VkResultToString(result), 1);
        
        vkDeviceWaitIdle(GetDevice());
        vkFreeCommandBuffers(GetDevice(), GetCommandPool(), 1, &commandBuffer);

        if(drawData->TotalVtxCount) {
            // Destroy the vertex and index buffers
            vkDestroyBuffer(GetDevice(), vertexBuffer, GetVulkanAllocator());
            vkDestroyBuffer(GetDevice(), indexBuffer, GetVulkanAllocator());
            vkFreeMemory(GetDevice(), vertexBufferMemory, GetVulkanAllocator());
            vkFreeMemory(GetDevice(), indexBufferMemory, GetVulkanAllocator());
        }
    }

    VkPipeline GetImGuiPipeline() {
        return pipeline;
    }
    VkPipelineLayout GetImGuiPipelineLayout() {
        return layout;
    }
}