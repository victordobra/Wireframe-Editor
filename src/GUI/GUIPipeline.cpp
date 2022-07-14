#include "GUI/GUIPipeline.hpp"
#include "Vulkan/Device.hpp"
#include "Vulkan/SwapChain.hpp"
#include "Config/EditorColors.hpp"
#include "MainWindow/MainWindow.hpp"

namespace wfe::editor {
    // Variables
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

    static void SetPipelineInfo() {
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
		vertexInputInfo.vertexAttributeDescriptionCount = 0;
		vertexInputInfo.vertexBindingDescriptionCount = 0;
		vertexInputInfo.pVertexAttributeDescriptions = nullptr;
		vertexInputInfo.pVertexBindingDescriptions = nullptr;
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

        createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.setLayoutCount = 0;
        createInfo.pSetLayouts = nullptr;
        createInfo.pushConstantRangeCount = 0;
        createInfo.pPushConstantRanges = nullptr;

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
    void CreateGUIPipeline() {
        SetPipelineInfo();
        CreatePipelineLayout();
        CreatePipeline();
    }
    void DeleteGUIPipeline() {
        vkDeviceWaitIdle(GetDevice());

        vkDestroyShaderModule(GetDevice(), shaderModules[0], nullptr);
        vkDestroyShaderModule(GetDevice(), shaderModules[1], nullptr);

        vkDestroyPipeline(GetDevice(), pipeline, nullptr);
        vkDestroyPipelineLayout(GetDevice(), layout, nullptr);
    }

    VkPipeline GetGUIPipeline() {
        return pipeline;
    }
    VkPipelineLayout GetGUIPipelineLayout() {
        return layout;
    }

    void Draw() {
        VkCommandBuffer commandBuffer;

        // Acquire the next image
        uint32_t imageIndex;

        auto result = AcquireNextImage(&imageIndex);
        if(result == VK_ERROR_OUT_OF_DATE_KHR)
            return;
        if(result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
            console::OutFatalError((string)"Failed to acquire the next image! Error code: " + VkResultToString(result), 1);
        
        // Allocate the command buffer
        VkCommandBufferAllocateInfo allocInfo;

        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.pNext = nullptr;
        allocInfo.commandPool = GetCommandPool();
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = 1;

        result = vkAllocateCommandBuffers(GetDevice(), &allocInfo, &commandBuffer);
        if(result != VK_SUCCESS)
            console::OutFatalError((string)"Failed to allocate the command buffer! Error code: " + VkResultToString(result), 1);
        
        // Begin recording the command buffer
        VkCommandBufferBeginInfo beginInfo;

        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.pNext = nullptr;
        beginInfo.flags = 0;
        beginInfo.pInheritanceInfo = nullptr;

        result = vkBeginCommandBuffer(commandBuffer, &beginInfo);
        if(result != VK_SUCCESS)
            console::OutFatalError((string)"Failed to begin recording the command buffer! Error code: " + VkResultToString(result), 1);
        
        // Begin the render pass
        VkRenderPassBeginInfo renderPassInfo;

        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.pNext = nullptr;
        renderPassInfo.renderPass = GetRenderPass();
        renderPassInfo.framebuffer = GetFrameBuffer((int32_t)imageIndex);
        renderPassInfo.renderArea = { { 0, 0 }, GetSwapChainExtent() };

        uint32_t backgroundColor = GetBackgroundColor();
        float32_t red = ((backgroundColor >> 16) & 0xff) / 255.f;
        float32_t green = ((backgroundColor >> 0) & 0xff) / 255.f;
        float32_t blue = (backgroundColor & 0xff) / 255.f;

        VkClearValue clearValues[2];
        clearValues[0].color = { red, green, blue, 1.f };
        clearValues[1].depthStencil = { 1.f, 0 };

        renderPassInfo.clearValueCount = 2;
        renderPassInfo.pClearValues = clearValues;

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        // Bind the pipeline
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

        // Set the viewport and scissor
        size_t windowWidth = GetMainWindowWidth();
        size_t windowHeight = GetMainWindowHeight();

        VkViewport viewport{ 0.f, 0.f, (float32_t)windowWidth, (float32_t)windowHeight, 0.f, 1.f };
        VkRect2D scissor{ { 0, 0 }, { (uint32_t)windowWidth, (uint32_t)windowHeight } };

        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

        // Draw the square
        vkCmdDraw(commandBuffer, 6, 1, 0, 0);

        // End the render pass
        vkCmdEndRenderPass(commandBuffer);

        // Stop recording the command buffer
        result = vkEndCommandBuffer(commandBuffer);
        if(result != VK_SUCCESS)
            console::OutFatalError((string)"Failed to end recording the command buffer! Error code: " + VkResultToString(result), 1);
        
        // Submit the command buffer
        result = SubmitCommandBuffers(&commandBuffer, &imageIndex);
        if(result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
            return;
        if(result != VK_SUCCESS)
            console::OutFatalError((string)"Failed to submit the command buffer! Error code: " + VkResultToString(result), 1);
        
        // Free the command buffer
        vkDeviceWaitIdle(GetDevice());
        vkFreeCommandBuffers(GetDevice(), GetCommandPool(), 1, &commandBuffer);
    }
}