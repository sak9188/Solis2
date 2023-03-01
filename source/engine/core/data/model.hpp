#pragma once

#include "core/base/solis_core.hpp"
#include "core/base/object.hpp"
#include "core/base/using.hpp"

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_NOEXCEPTION
#define JSON_NOEXCEPTION
#include "tiny_gltf.h"

#include "core/data/mesh.hpp"

namespace solis
{
    class Model : public Object<Model>
    {
    public:
        Model(const string &file_name);
        ~Model();

    private:
        void LoadMeshes(tinygltf::Model &model);

    private:
        vector<std::shared_ptr<Mesh>> mMeshes;
    };
}