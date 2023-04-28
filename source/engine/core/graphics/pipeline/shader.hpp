#pragma once

#include "core/solis_core.hpp"
#include "core/base/object.hpp"
#include "core/base/using.hpp"

#include "volk.h"

namespace solis {
namespace graphics {
// TODO:: ShaderVariant

class SOLIS_CORE_API Shader : public Object<Shader>
{
public:
    enum class Type : uint32_t
    {
        Vertex                 = VK_SHADER_STAGE_VERTEX_BIT,
        TessellationControl    = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT,
        TessellationEvaluation = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT,
        Geometry               = VK_SHADER_STAGE_GEOMETRY_BIT,
        Fragment               = VK_SHADER_STAGE_FRAGMENT_BIT,
        Compute                = VK_SHADER_STAGE_COMPUTE_BIT,

        // 下面的都是扩展的shader stage
        // Raygen = VK_SHADER_STAGE_RAYGEN_BIT_KHR,
        // AnyHit = VK_SHADER_STAGE_ANY_HIT_BIT_KHR,
        // ClosestHit = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR,
        // Miss = VK_SHADER_STAGE_MISS_BIT_KHR,
        // Intersection = VK_SHADER_STAGE_INTERSECTION_BIT_KHR,
        // Callable = VK_SHADER_STAGE_CALLABLE_BIT_KHR,
        // Task = VK_SHADER_STAGE_TASK_BIT_NV,
        // Mesh = VK_SHADER_STAGE_MESH_BIT_NV,
        Count
    };

    Shader() = default;
    virtual ~Shader();

    // glsl, spv
    void CreateShaderModule(const string &codePath, Type type);
    void CreateShaderModule(const vector<uint8_t> &code, Type type);
    void CreateShaderModule(const vector<uint32_t> &code, Type type);

    const VkShaderModule GetShaderModule(Type type) const
    {
        return mShaderModules[(uint32_t)type];
    }

    VkPipelineShaderStageCreateInfo GetPipelineShaderStageCreateInfo(Type type) const
    {
        VkPipelineShaderStageCreateInfo createInfo = {};
        createInfo.sType                           = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        createInfo.stage                           = (VkShaderStageFlagBits)(uint32_t)type;
        createInfo.module                          = mShaderModules[(uint32_t)type];
        createInfo.pName                           = "main";
        return createInfo;
    }

    vector<VkPipelineShaderStageCreateInfo> GetPipelineShaderStageCreateInfos() const
    {
        vector<VkPipelineShaderStageCreateInfo> createInfos;
        for (uint32_t i = 0; i < (uint32_t)Type::Count; ++i)
        {
            if (mShaderModules[i] != VK_NULL_HANDLE)
            {
                createInfos.push_back(GetPipelineShaderStageCreateInfo((Type)i));
            }
        }
        return createInfos;
    }

private:
    vector<VkShaderModule> mShaderModules{(uint32_t)Type::Count};
};
}
} // namespace solis::graphics