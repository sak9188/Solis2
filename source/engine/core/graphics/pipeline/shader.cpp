#include "core/graphics/pipeline/shader.hpp"
#include "core/graphics/graphics.hpp"
#include "core/graphics/logical_device.hpp"

#include "core/files/files.hpp"
#include "core/files/file_info.hpp"

#include "core/log/log.hpp"

#include "glslang/Public/ShaderLang.h"
#include "glslang/Public/ResourceLimits.h"
#include "glslang/SPIRV/GlslangToSpv.h"

namespace solis {
namespace graphics {
EShLanguage ConvertShaderType(Shader::Type type)
{
    switch (type)
    {
    case Shader::Type::Vertex:
        return EShLangVertex;
    case Shader::Type::TessellationControl:
        return EShLangTessControl;
    case Shader::Type::TessellationEvaluation:
        return EShLangTessEvaluation;
    case Shader::Type::Fragment:
        return EShLangFragment;
    case Shader::Type::Compute:
        return EShLangCompute;
    default:
        return EShLangCount;
    }
}

vector<uint32_t> CompileShaderSource(const vector<uint8_t> &source, Shader::Type type)
{
    glslang::InitializeProcess();
    EShMessages messages = static_cast<EShMessages>(EShMsgDefault | EShMsgVulkanRules | EShMsgSpvRules | EShMsgDebugInfo);
    EShLanguage stage    = ConvertShaderType(type);

    glslang::TShader shader(stage);
    shader.setEntryPoint("main");
    shader.setSourceEntryPoint("main");

    auto sourceString = reinterpret_cast<const char *>(source.data());
    auto sourceLength = (int)source.size();
    shader.setStringsWithLengths(&sourceString, &sourceLength, 1);
    shader.setEnvTarget(glslang::EShTargetLanguage::EShTargetSpv, glslang::EShTargetSpv_1_0);

    // 这个是设置前置宏, 目前用不到
    // shader.setPreamble();

    // 100 ES, 110以上 desktop
    // 暂时不考虑移动端
    if (!shader.parse(GetDefaultResources(), 100, false, messages))
    {
        Log::SError("Shader::CompileShaderSource: shader parse failed info: {}", shader.getInfoLog());
        return {};
    }

    glslang::TProgram program;
    program.addShader(&shader);

    if (!program.link(messages))
    {
        Log::SError("Shader::CompileShaderSource: shader compilation failed info: {}", program.getInfoLog());
        return {};
    }

    if (shader.getInfoLog()[0] != 0)
    {
        Log::SInfo(shader.getInfoLog());
    }

    if (shader.getInfoDebugLog()[0] != 0)
    {
        Log::SDebug(shader.getInfoDebugLog());
    }

    if (program.getInfoLog()[0] != 0)
    {
        Log::SInfo(program.getInfoLog());
    }

    if (program.getInfoDebugLog()[0] != 0)
    {
        Log::SDebug(program.getInfoDebugLog());
    }

    std::vector<unsigned int> spirv;
    spv::SpvBuildLogger       logger;

    glslang::TIntermediate *intermediate = program.getIntermediate(stage);

    if (!intermediate)
    {
        Log::SError("Shader::CompileShaderSource: shader compilation failed");
        return {};
    }

    glslang::GlslangToSpv(*intermediate, spirv, &logger);

    if (logger.getAllMessages().length() > 0)
        Log::SInfo(logger.getAllMessages());

    glslang::FinalizeProcess();

    return {spirv.begin(), spirv.end()};
}

Shader::~Shader()
{
    for (auto &shaderModule : mShaderModules)
    {
        if (shaderModule != VK_NULL_HANDLE)
        {
            vkDestroyShaderModule(*Graphics::Get()->GetLogicalDevice(), shaderModule, nullptr);
            shaderModule = VK_NULL_HANDLE;
        }
    }
}

void Shader::CreateShaderModule(const string &codePath, Type type)
{
    using namespace files;
    FileInfo fileInfo(codePath);

    vector<uint32_t> code;

    if (!fileInfo.Exist())
    {
        Log::SError("Shader::CreateShaderModule: shader file not exists path:{}", codePath);
        return;
    }

    if (fileInfo.GetExtension() == ".vert" || fileInfo.GetExtension() == ".frag" || fileInfo.GetExtension() == ".comp")
    {
        vector<uint8_t> source = fileInfo.ReadBytes();
        code                   = CompileShaderSource(source, type);
    }
    else if (fileInfo.GetExtension() == ".spv")
    {
        // code.insert(code.end(), reinterpret_cast<const uint32_t *>(fileInfo.ReadBytes().data()),
        // reinterpret_cast<const uint32_t *>(fileInfo.ReadBytes().data()) + fileInfo.ReadBytes().size() / sizeof(uint32_t));
        CreateShaderModule(fileInfo.ReadBytes(), type);
        return;
    }
    else
    {
        Log::SError("Shader::CreateShaderModule: shader file extension must be glsl or spv");
        return;
    }

    CreateShaderModule(code, type);
}

void Shader::CreateShaderModule(const vector<uint8_t> &code, Type type)
{
    if (code.empty())
    {
        Log::SError("Shader::CreateShaderModule: shader code is empty");
        return;
    }
    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType                    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize                 = code.size();
    createInfo.pCode                    = reinterpret_cast<const uint32_t *>(code.data());

    Graphics::CheckVk(vkCreateShaderModule(
        *Graphics::Get()->GetLogicalDevice(), &createInfo, nullptr, &mShaderModules[(uint32_t)type]));
}

void Shader::CreateShaderModule(const vector<uint32_t> &code, Type type)
{
    if (code.empty())
    {
        Log::SError("Shader::CreateShaderModule: shader code is empty");
        return;
    }
    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType                    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize                 = code.size() * sizeof(uint32_t);
    createInfo.pCode                    = code.data();

    Graphics::CheckVk(vkCreateShaderModule(
        *Graphics::Get()->GetLogicalDevice(), &createInfo, nullptr, &mShaderModules[(uint32_t)type]));
}
}
} // namespace solis::graphics