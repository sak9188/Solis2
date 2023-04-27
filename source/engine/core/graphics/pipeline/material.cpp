#include "core/graphics/pipeline/material.hpp"

#include "core/files/files.hpp"

namespace solis::graphics {
Material::Material(const string &shaderDirPath)
{
    auto vs = files::ReadTextFile(shaderDirPath + "/vs.glsl");
    auto ps = files::ReadTextFile(shaderDirPath + "/ps.glsl");
}

Material::Material(const string &vs, const string &ps)
{
}
} // namespace solis::graphics