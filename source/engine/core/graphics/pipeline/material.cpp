#include "core/graphics/pipeline/material.hpp"

#include "core/files/files.hpp"
#include "core/graphics/pipeline/pipeline_graphics.hpp"

namespace solis::graphics {
Material::Material(const string &shaderDirPath, const string &passName) :
    mPassNodeName(passName)
{
    // 默认没有cs
    auto fs  = files::Files::Get();
    auto vs  = fs->ReadFile(shaderDirPath + "/vs.glsl");
    auto tsc = fs->ReadFile(shaderDirPath + "/tsc.glsl");
    auto tse = fs->ReadFile(shaderDirPath + "/tse.glsl");
    auto gs  = fs->ReadFile(shaderDirPath + "/gs.glsl");
    auto ps  = fs->ReadFile(shaderDirPath + "/ps.glsl");

    mPipeline    = std::make_unique<PipelineGraphics>();
    auto &shader = mPipeline->GetShader();

    shader.CreateShaderModule(vs, Shader::Type::Vertex);
    shader.CreateShaderModule(tsc, Shader::Type::TessellationControl);
    shader.CreateShaderModule(tse, Shader::Type::TessellationEvaluation);
    shader.CreateShaderModule(gs, Shader::Type::Geometry);
    shader.CreateShaderModule(ps, Shader::Type::Fragment);

    mPipeline->Build(passName);
}

Material::Material(const string &vs, const string &ps, const string &passName) :
    mPassNodeName(passName)
{
    mPipeline    = std::make_unique<PipelineGraphics>();
    auto &shader = mPipeline->GetShader();

    shader.CreateShaderModule(vs, Shader::Type::Vertex);
    shader.CreateShaderModule(ps, Shader::Type::Fragment);

    mPipeline->Build(passName);
}
} // namespace solis::graphics