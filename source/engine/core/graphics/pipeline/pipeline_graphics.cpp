#include "core/graphics/pipeline/pipeline_graphics.hpp"
#include "core/graphics/logical_device.hpp"
#include "core/graphics/image/image.hpp"

#include "core/log/log.hpp"

#include "core/data/mesh.hpp"
#include "core/data/texture.hpp"

namespace solis {
namespace graphics {
PipelineGraphics::PipelineGraphics()
{
    std::array<VkDescriptorPoolSize, 2> poolSizes = {};
    poolSizes[0].type                             = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount                  = MaxFrameInFlight;
    poolSizes[1].type                             = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[1].descriptorCount                  = MaxFrameInFlight;

    VkDescriptorPoolCreateInfo poolInfo = {};
    poolInfo.sType                      = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount              = poolSizes.size();
    poolInfo.pPoolSizes                 = poolSizes.data();
    poolInfo.maxSets                    = MaxFrameInFlight; // todo: 这里要根据shader的descriptor数量来设置 *

    Graphics::CheckVk(vkCreateDescriptorPool(*Graphics::Get()->GetLogicalDevice(), &poolInfo, nullptr, &mDescriptorPool));

    // 创建Pipeline DescriptorSetLayout
    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding            = 0;
    uboLayoutBinding.descriptorCount    = 1; // todo: 这里要根据shader的descriptor数量来设置
    uboLayoutBinding.descriptorType     = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.pImmutableSamplers = nullptr;
    uboLayoutBinding.stageFlags         = VK_SHADER_STAGE_VERTEX_BIT; // 这里需要根据具体的shader来设置

    // sampler layout
    VkDescriptorSetLayoutBinding samplerLayoutBinding{};
    samplerLayoutBinding.binding            = 1;
    samplerLayoutBinding.descriptorCount    = 1; // todo: 这里要根据shader的descriptor数量来设置
    samplerLayoutBinding.descriptorType     = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerLayoutBinding.pImmutableSamplers = nullptr;
    samplerLayoutBinding.stageFlags         = VK_SHADER_STAGE_FRAGMENT_BIT; // 这里需要根据具体的shader来设置

    std::array<VkDescriptorSetLayoutBinding, 2> bindings = {uboLayoutBinding, samplerLayoutBinding};

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size()); // todo: 这里应该通过反射来获取
    layoutInfo.pBindings    = bindings.data();

    Graphics::CheckVk(vkCreateDescriptorSetLayout(*Graphics::Get()->GetLogicalDevice(), &layoutInfo, nullptr, &mDescriptorSetLayout));

    // Allocate DescriptorSet
    vector<VkDescriptorSetLayout> layouts(MaxFrameInFlight, mDescriptorSetLayout);
    VkDescriptorSetAllocateInfo   allocInfo{};
    allocInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool     = mDescriptorPool;
    allocInfo.descriptorSetCount = layouts.size(); // todo: 这里要根据shader的descriptor数量来设置
    allocInfo.pSetLayouts        = layouts.data();

    mDescriptorSets.resize(MaxFrameInFlight);
    Graphics::CheckVk(vkAllocateDescriptorSets(*Graphics::Get()->GetLogicalDevice(), &allocInfo, mDescriptorSets.data()));
}

PipelineGraphics::~PipelineGraphics()
{
    Destroy();
}

void PipelineGraphics::Destroy()
{
    if (IsDestroyed())
    {
        return;
    }

    Pipeline::Destroy();

    auto device = Graphics::Get()->GetLogicalDevice();

    mShader.reset();

    if (mPipeline != VK_NULL_HANDLE)
    {
        vkDestroyPipeline(*device, mPipeline, nullptr);
        mPipeline = VK_NULL_HANDLE;
    }

    if (mPipelineLayout != VK_NULL_HANDLE)
    {
        vkDestroyPipelineLayout(*device, mPipelineLayout, nullptr);
        mPipelineLayout = VK_NULL_HANDLE;
    }

    if (mDescriptorPool != VK_NULL_HANDLE)
    {
        vkDestroyDescriptorPool(*device, mDescriptorPool, nullptr);
        mDescriptorPool = VK_NULL_HANDLE;
    }

    if (mDescriptorSetLayout != VK_NULL_HANDLE)
    {
        vkDestroyDescriptorSetLayout(*device, mDescriptorSetLayout, nullptr);
        mDescriptorSetLayout = VK_NULL_HANDLE;
    }

    mDestroyed = true;
}

bool PipelineGraphics::Build(const RenderPass &renderPass, size_t subpassIndex)
{
    VkVertexInputBindingDescription bindingDescription{};
    bindingDescription.binding   = 0;
    bindingDescription.stride    = sizeof(Vertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};
    attributeDescriptions[0].binding  = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format   = VK_FORMAT_R32G32B32A32_SFLOAT;
    attributeDescriptions[0].offset   = offsetof(Vertex, position);

    attributeDescriptions[1].binding  = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format   = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[1].offset   = offsetof(Vertex, normal);

    attributeDescriptions[2].binding  = 0;
    attributeDescriptions[2].location = 2;
    attributeDescriptions[2].format   = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[2].offset   = offsetof(Vertex, texcoord);

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount   = 1;
    vertexInputInfo.vertexAttributeDescriptionCount = (uint32_t)attributeDescriptions.size();
    vertexInputInfo.pVertexBindingDescriptions      = &bindingDescription;
    vertexInputInfo.pVertexAttributeDescriptions    = attributeDescriptions.data();

    // 这里是输入的拓扑结构
    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    // 这里是视口
    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount  = 1;

    // 这里是光栅化
    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable        = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode             = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth               = 1.0f;
    rasterizer.cullMode                = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace               = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable         = VK_FALSE;

    // 这里是设置多重采样
    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType                = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable  = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    // 这里是颜色混合模式
    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable    = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType             = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable     = VK_FALSE;
    colorBlending.logicOp           = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount   = 1;
    colorBlending.pAttachments      = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    // 可以动态控制的State
    vector<VkDynamicState> dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR};

    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates    = dynamicStates.data();

    // 创建Pipeline DescriptorSetLayout
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount         = 1;
    pipelineLayoutInfo.pSetLayouts            = &mDescriptorSetLayout;
    pipelineLayoutInfo.pushConstantRangeCount = 0;

    if (vkCreatePipelineLayout(*Graphics::Get()->GetLogicalDevice(), &pipelineLayoutInfo, nullptr, &mPipelineLayout) != VK_SUCCESS)
    {
        Log::SError("failed to create pipeline layout!");
        return false;
    }

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    auto shaderStages                = mShader->GetPipelineShaderStageCreateInfos();
    pipelineInfo.stageCount          = static_cast<uint32_t>(shaderStages.size());
    pipelineInfo.pStages             = shaderStages.data();
    pipelineInfo.pVertexInputState   = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState      = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState   = &multisampling;
    pipelineInfo.pDepthStencilState  = nullptr;
    pipelineInfo.pColorBlendState    = &colorBlending;
    pipelineInfo.pDynamicState       = &dynamicState;
    pipelineInfo.layout              = mPipelineLayout;
    pipelineInfo.renderPass          = renderPass.GetRenderPass();
    // 这里是指代的是RenderPass中Subpass的索引
    // 这里暂时就是这样
    pipelineInfo.subpass            = subpassIndex;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    if (vkCreateGraphicsPipelines(*Graphics::Get()->GetLogicalDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &mPipeline) != VK_SUCCESS)
    {
        Log::SError("failed to create graphics pipeline!");
        return false;
    }

    return true;
}

// 暂时这么做， 以后再改
void PipelineGraphics::BindEverything(Texture &texture, size_t index)
{
    for (int i = 0; i < MaxFrameInFlight; i++)
    {
        std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

        auto &ubo = GetUniformBuffer(i);

        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = ubo.GetBuffer();
        bufferInfo.offset = 0;
        bufferInfo.range  = ubo.GetSize();

        descriptorWrites[0].sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[0].dstSet          = mDescriptorSets[i];
        descriptorWrites[0].dstBinding      = 0;
        descriptorWrites[0].dstArrayElement = 0;
        descriptorWrites[0].descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].pBufferInfo     = &bufferInfo;

        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView   = texture.GetImage().GetView();
        imageInfo.sampler     = texture.GetImage().GetSampler();

        descriptorWrites[1].sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[1].dstSet          = mDescriptorSets[i];
        descriptorWrites[1].dstBinding      = index;
        descriptorWrites[1].dstArrayElement = 0;
        descriptorWrites[1].descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites[1].descriptorCount = 1;
        descriptorWrites[1].pImageInfo      = &imageInfo;

        // 有些UnionBuffer
        vkUpdateDescriptorSets(*Graphics::Get()->GetLogicalDevice(), descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
    }
}
}
} // namespace solis::graphics