#include "ImGui/ImGuiPipeline.hpp"
#include "ImGui/ImGuiContext.hpp"
#include "Vulkan/Device.hpp"
#include "Vulkan/SwapChain.hpp"

namespace wfe::editor {
    // Structs
    struct Vertex {
        float32_t position[2];
        float32_t uvCoord[2];
        float32_t color[4];
    };
    struct PushConstants {
        float32_t position[2];
    };

    // Constants
    const size_t MAX_DESCRIPTOR_COUNT = 256;

    // Variables
    VkVertexInputBindingDescription vertexBinding;
    vector<VkVertexInputAttributeDescription> vertexAttributes;

    VkPipelineViewportStateCreateInfo viewportInfo;
    VkPipelineVertexInputStateCreateInfo vertexInputInfo;
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
    VkPipelineRasterizationStateCreateInfo rasterizationInfo;
    VkPipelineMultisampleStateCreateInfo multisampleInfo;
    VkPipelineColorBlendAttachmentState colorBlendAttachment;
    VkPipelineColorBlendStateCreateInfo colorBlendInfo;
    VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
    VkPipelineDynamicStateCreateInfo dynamicStateInfo;
    vector<VkDynamicState> dynamicStates;
    uint32_t subpass = 0;

    VkShaderModule shaderModules[2];

    VkPipeline pipeline;
    VkPipelineLayout layout;

    VkSampler sampler;

    VkDescriptorPool descriptorPool;
    VkDescriptorSetLayout fontDescriptorSetLayout;
    VkDescriptorSet fontDescriptorSets[MAX_FRAMES_IN_FLIGHT];

    VkImage fontImages[MAX_FRAMES_IN_FLIGHT];
    VkDeviceMemory fontImageMemories[MAX_FRAMES_IN_FLIGHT];
    VkImageView fontImageViews[MAX_FRAMES_IN_FLIGHT];

    // Internal helper functions
    static void CreateShaderModule(const string& shaderLocation, VkShaderModule* module) {
        // Load the shader from the file
        FileInput fileInput(shaderLocation, (StreamType)(STREAM_TYPE_BINARY | STREAM_TYPE_AT_THE_END));
        size_t shaderSize = fileInput.Tell();
        fileInput.Seek(0);

        char_t* shaderData = new char_t[shaderSize];
        fileInput.ReadBuffer(shaderData, shaderSize * sizeof(char_t));

        fileInput.Close();

        // Create the shader module
        VkShaderModuleCreateInfo createInfo;

        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.codeSize = (uint32_t)shaderSize;
        createInfo.pCode = (const uint32_t*)shaderData;

        auto result = vkCreateShaderModule(GetDevice(), &createInfo, nullptr, module);
        if(result != VK_SUCCESS)
            console::OutFatalError((string)"Failed to create shader module! Error code: " + VkResultToString(result), 1);
    }

    static void CreateSampler() {
        // Create the sampler info
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
        createInfo.maxAnisotropy = 1.0f;
        createInfo.compareEnable = VK_FALSE;
        createInfo.compareOp = VK_COMPARE_OP_NEVER;
        createInfo.minLod = -1000.f;
        createInfo.maxLod = 1000.f;
        createInfo.borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
        createInfo.unnormalizedCoordinates = VK_FALSE;

        // Create the sampler
        auto result = vkCreateSampler(GetDevice(), &createInfo, nullptr, &sampler);
        if(result != VK_SUCCESS)
            console::OutFatalError((string)"Failed to create sampler! Error code: " + VkResultToString(result), 1);
    }
    static void CreateDescriptorPool() {
        // Create the descriptor pool
        VkDescriptorPoolSize poolSizes[] = {
            { VK_DESCRIPTOR_TYPE_SAMPLER, MAX_DESCRIPTOR_COUNT },
            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, MAX_DESCRIPTOR_COUNT },
            { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, MAX_DESCRIPTOR_COUNT },
            { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, MAX_DESCRIPTOR_COUNT },
            { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, MAX_DESCRIPTOR_COUNT },
            { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, MAX_DESCRIPTOR_COUNT },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, MAX_DESCRIPTOR_COUNT },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, MAX_DESCRIPTOR_COUNT },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, MAX_DESCRIPTOR_COUNT },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, MAX_DESCRIPTOR_COUNT },
            { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, MAX_DESCRIPTOR_COUNT }
        };

        VkDescriptorPoolCreateInfo descriptorPoolCreateInfo;

        descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptorPoolCreateInfo.pNext = nullptr;
        descriptorPoolCreateInfo.flags = 0;
        descriptorPoolCreateInfo.maxSets = MAX_DESCRIPTOR_COUNT * 11;
        descriptorPoolCreateInfo.poolSizeCount = 11;
        descriptorPoolCreateInfo.pPoolSizes = poolSizes;

        auto result = vkCreateDescriptorPool(GetDevice(), &descriptorPoolCreateInfo, nullptr, &descriptorPool);
        if(result != VK_SUCCESS)
            console::OutFatalError((string)"Failed to create descriptor pool! Error code: " + VkResultToString(result), 1);
        
        // Create the descriptor set
        VkDescriptorSetLayoutBinding fontDescriptorSetLayoutBinding;

        fontDescriptorSetLayoutBinding.binding = 0;
        fontDescriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        fontDescriptorSetLayoutBinding.descriptorCount = 1;
        fontDescriptorSetLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        fontDescriptorSetLayoutBinding.pImmutableSamplers = nullptr;

        // Create the descriptor set layout
        VkDescriptorSetLayoutCreateInfo fontDescriptorSetLayoutCreateInfo;

        fontDescriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        fontDescriptorSetLayoutCreateInfo.pNext = nullptr;
        fontDescriptorSetLayoutCreateInfo.flags = 0;
        fontDescriptorSetLayoutCreateInfo.bindingCount = 1;
        fontDescriptorSetLayoutCreateInfo.pBindings = &fontDescriptorSetLayoutBinding;

        result = vkCreateDescriptorSetLayout(GetDevice(), &fontDescriptorSetLayoutCreateInfo, nullptr, &fontDescriptorSetLayout);
        if(result != VK_SUCCESS)
            console::OutFatalError((string)"Failed to create descriptor set layout! Error code: " + VkResultToString(result), 1);
        
        // Create the descriptor set
        VkDescriptorSetLayout layouts[MAX_FRAMES_IN_FLIGHT];
        for(size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
            layouts[i] = fontDescriptorSetLayout;

        VkDescriptorSetAllocateInfo fontDescriptorSetAllocInfo;

        fontDescriptorSetAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        fontDescriptorSetAllocInfo.pNext = nullptr;
        fontDescriptorSetAllocInfo.descriptorPool = descriptorPool;
        fontDescriptorSetAllocInfo.descriptorSetCount = MAX_FRAMES_IN_FLIGHT;
        fontDescriptorSetAllocInfo.pSetLayouts = layouts;

        result = vkAllocateDescriptorSets(GetDevice(), &fontDescriptorSetAllocInfo, fontDescriptorSets);
        if(result != VK_SUCCESS)
            console::OutFatalError((string)"Failed to create descriptor set layout! Error code: " + VkResultToString(result), 1);
    }
    static void CreateFontTextures() {
        // Load the font texture data
        ImGuiIO* io = GetImGuiIO();

        uint8_t* data;
        int32_t width, height, pixelSize;
        io->Fonts->GetTexDataAsRGBA32(&data, &width, &height, &pixelSize);

        // Create the staging buffer
        VkBuffer buffer;
        VkDeviceMemory bufferMemory;
        VkDeviceSize bufferSize = width * height * pixelSize;

        CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, buffer, bufferMemory);

        // Copy to the staging buffer
        void* bufferMappedMemory;

        vkMapMemory(GetDevice(), bufferMemory, 0, bufferSize, 0, &bufferMappedMemory);

        memcpy(bufferMappedMemory, data, (size_t)bufferSize);

        VkMappedMemoryRange mappedMemoryRange;

        mappedMemoryRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        mappedMemoryRange.pNext = nullptr;
        mappedMemoryRange.memory = bufferMemory;
        mappedMemoryRange.offset = 0;
        mappedMemoryRange.size = bufferSize;

        vkFlushMappedMemoryRanges(GetDevice(), 1, &mappedMemoryRange);

        vkUnmapMemory(GetDevice(), bufferMemory);

        // Set the image and image view create infos
        VkImageCreateInfo imageCreateInfo;

        imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageCreateInfo.pNext = nullptr;
        imageCreateInfo.flags = 0;
        imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
        imageCreateInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
        imageCreateInfo.extent = { (uint32_t)width, (uint32_t)height, 1 };
        imageCreateInfo.mipLevels = 1;
        imageCreateInfo.arrayLayers = 1;
        imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageCreateInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageCreateInfo.queueFamilyIndexCount = 0;
        imageCreateInfo.pQueueFamilyIndices = nullptr;
        imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

        VkImageViewCreateInfo imageViewCreateInfo;

        imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewCreateInfo.pNext = nullptr;
        imageViewCreateInfo.flags = 0;
        imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageViewCreateInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
        imageViewCreateInfo.components = { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY };

        imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
        imageViewCreateInfo.subresourceRange.levelCount = 1;
        imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        imageViewCreateInfo.subresourceRange.layerCount = 1;

        for(size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
            // Create the image and copy the buffer contents to it
            CreateImage(imageCreateInfo, VK_MEMORY_HEAP_DEVICE_LOCAL_BIT, fontImages[i], fontImageMemories[i]);

            imageViewCreateInfo.image = fontImages[i];
            auto result = vkCreateImageView(GetDevice(), &imageViewCreateInfo, nullptr, fontImageViews + i);
            if(result != VK_SUCCESS)
                console::OutFatalError((string)"Failed to create image view! Error code: " + VkResultToString(result), 1);
            
            TransitionImageLayout(fontImages[i], VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_FORMAT_R8G8B8A8_UNORM);
            CopyBufferToImage(buffer, fontImages[i], (uint32_t)width, (uint32_t)height, 1);
            TransitionImageLayout(fontImages[i], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_FORMAT_R8G8B8A8_UNORM);

            // Write the the appropriate descriptor set
            VkDescriptorImageInfo imageInfo;
            imageInfo.sampler = sampler;
            imageInfo.imageView = fontImageViews[i];
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

            VkWriteDescriptorSet write;

            write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            write.pNext = nullptr;
            write.dstSet = fontDescriptorSets[i];
            write.dstBinding = 0;
            write.dstArrayElement = 0;
            write.descriptorCount = 1;
            write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            write.pImageInfo = &imageInfo;
            write.pBufferInfo = nullptr;
            write.pTexelBufferView = nullptr;

            vkUpdateDescriptorSets(GetDevice(), 1, &write, 0, nullptr);
        }

        // Delete the staging buffer
        vkDestroyBuffer(GetDevice(), buffer, nullptr);
        vkFreeMemory(GetDevice(), bufferMemory, nullptr);
    }
    static void SetPipelineInfo() {
        // Vertex binding and attributes
        vertexBinding.binding = 0;
        vertexBinding.stride = sizeof(Vertex);
        vertexBinding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        vertexAttributes.resize(3);
        vertexAttributes[0] = { 0, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, position) };
        vertexAttributes[1] = { 1, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uvCoord) };
        vertexAttributes[2] = { 2, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Vertex, color) };

        // Viewport info
		viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportInfo.viewportCount = 1;
		viewportInfo.pViewports = nullptr;
		viewportInfo.scissorCount = 1;
		viewportInfo.pScissors = nullptr;
		viewportInfo.flags = 0;
		viewportInfo.pNext = nullptr;

		// Vertex input info
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexAttributeDescriptionCount = (uint32_t)vertexAttributes.size();
        vertexInputInfo.pVertexAttributeDescriptions = vertexAttributes.data();
		vertexInputInfo.vertexBindingDescriptionCount = 1;
		vertexInputInfo.pVertexBindingDescriptions = &vertexBinding;
		vertexInputInfo.flags = 0;
		vertexInputInfo.pNext = nullptr;

        // Input assembly info
		inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;
		inputAssemblyInfo.flags = 0;
		inputAssemblyInfo.pNext = nullptr;

		// Rasterization info
		rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizationInfo.depthClampEnable = VK_FALSE;
		rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
		rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizationInfo.lineWidth = 1.f;
		rasterizationInfo.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
		rasterizationInfo.depthBiasEnable = VK_FALSE;
		rasterizationInfo.depthBiasConstantFactor = 0.f;
		rasterizationInfo.depthBiasClamp = 0.f;
		rasterizationInfo.depthBiasSlopeFactor = 0.f;
		rasterizationInfo.flags = 0;
		rasterizationInfo.pNext = nullptr;

		// Multisample info
		multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampleInfo.sampleShadingEnable = VK_FALSE;
		multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisampleInfo.minSampleShading = 1.0f;
		multisampleInfo.pSampleMask = nullptr;
		multisampleInfo.alphaToCoverageEnable = VK_FALSE;
		multisampleInfo.alphaToOneEnable = VK_FALSE;
		multisampleInfo.flags = 0;
		multisampleInfo.pNext = nullptr;

		// Color blend attachmemt
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_FALSE;
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

		// Color blend info
		colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlendInfo.logicOpEnable = VK_FALSE;
		colorBlendInfo.logicOp = VK_LOGIC_OP_COPY;
		colorBlendInfo.attachmentCount = 1;
		colorBlendInfo.pAttachments = &colorBlendAttachment;
		colorBlendInfo.blendConstants[0] = 0.0f;
		colorBlendInfo.blendConstants[1] = 0.0f;
		colorBlendInfo.blendConstants[2] = 0.0f;
		colorBlendInfo.blendConstants[3] = 0.0f;
		colorBlendInfo.flags = 0;
		colorBlendInfo.pNext = nullptr;

		// Depth stencil info
		depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencilInfo.depthTestEnable = VK_TRUE;
		depthStencilInfo.depthWriteEnable = VK_TRUE;
		depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
		depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
		depthStencilInfo.minDepthBounds = 0.0f;
		depthStencilInfo.maxDepthBounds = 1.0f;
		depthStencilInfo.stencilTestEnable = VK_FALSE;
		depthStencilInfo.front = {};
		depthStencilInfo.back = {};
		depthStencilInfo.flags = 0;
		depthStencilInfo.pNext = nullptr;

		// Dynamic state info
		dynamicStates = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

		dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicStateInfo.dynamicStateCount = dynamicStates.size();
		dynamicStateInfo.pDynamicStates = dynamicStates.data();
		dynamicStateInfo.flags = 0;
		dynamicStateInfo.pNext = nullptr;
    }
    static void CreatePipelineLayout() {
        // Set the pipeline layout info
        VkPipelineLayoutCreateInfo createInfo;

        VkPushConstantRange pushConstantRange;
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(PushConstants);

        createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.setLayoutCount = 1;
        createInfo.pSetLayouts = &fontDescriptorSetLayout;
        createInfo.pushConstantRangeCount = 1;
        createInfo.pPushConstantRanges = &pushConstantRange;

        // Create the pipeline layout
        auto result = vkCreatePipelineLayout(GetDevice(), &createInfo, nullptr, &layout);
        if(result != VK_SUCCESS)
            console::OutFatalError((string)"Failed to create pipeline layout! Error code: " + VkResultToString(result), 1);
    }
    static void CreatePipeline() {
        // Create the shader modules
        CreateShaderModule("shaders/VertShader.vert.spv", shaderModules);
        CreateShaderModule("shaders/FragShader.frag.spv", shaderModules + 1);

        // Create the shader stages
        VkPipelineShaderStageCreateInfo shaderStages[2];

        for(size_t i = 0; i < 2; ++i) {
            shaderStages[i].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            shaderStages[i].pNext = nullptr;
            shaderStages[i].flags = 0;
            shaderStages[i].stage = i ? VK_SHADER_STAGE_FRAGMENT_BIT : VK_SHADER_STAGE_VERTEX_BIT;
            shaderStages[i].module = shaderModules[i];
            shaderStages[i].pName = "main";
            shaderStages[i].pSpecializationInfo = nullptr;
        }

        // Set the pipeline create info
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
        createInfo.subpass = subpass;

        createInfo.basePipelineHandle = VK_NULL_HANDLE;
        createInfo.basePipelineIndex = -1;

        // Create the pipeline
        auto result = vkCreateGraphicsPipelines(GetDevice(), VK_NULL_HANDLE, 1, &createInfo, nullptr, &pipeline);
        if(result != VK_SUCCESS)
            console::OutFatalError((string)"Failed to create graphics pipeline! Error code: " + VkResultToString(result), 1);
    }

    // Public functions
    void CreateImGuiPipeline() {
        CreateSampler();
        CreateDescriptorPool();
        CreateFontTextures();
        SetPipelineInfo();
        CreatePipelineLayout();
        CreatePipeline();
    }
    void DeleteImGuiPipeline() {
        vkDeviceWaitIdle(GetDevice());
        vkDestroyPipeline(GetDevice(), pipeline, nullptr);
        vkDestroyPipelineLayout(GetDevice(), layout, nullptr);

        vkDestroyShaderModule(GetDevice(), shaderModules[0], nullptr);
        vkDestroyShaderModule(GetDevice(), shaderModules[1], nullptr);

        vkDestroyImageView(GetDevice(), fontImageViews[0], nullptr);
        vkDestroyImageView(GetDevice(), fontImageViews[1], nullptr);

        vkDestroyImage(GetDevice(), fontImages[0], nullptr);
        vkDestroyImage(GetDevice(), fontImages[1], nullptr);

        vkFreeMemory(GetDevice(), fontImageMemories[0], nullptr);
        vkFreeMemory(GetDevice(), fontImageMemories[1], nullptr);

        vkDestroyDescriptorSetLayout(GetDevice(), fontDescriptorSetLayout, nullptr);
        vkDestroyDescriptorPool(GetDevice(), descriptorPool, nullptr);
        vkDestroySampler(GetDevice(), sampler, nullptr);
    }

    void RenderImGui() {

    }

    VkPipeline GetPipeline() {
        return pipeline;
    }
    VkPipelineLayout GetPipelineLayout() {
        return layout;
    }
    VkDescriptorPool GetDescriptorPool() {
        return descriptorPool;
    }
}