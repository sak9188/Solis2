#pragma once

#include "core/solis_core.hpp"
#include "core/base/object.hpp"
#include "core/base/using.hpp"

#include "core/data/mesh.hpp"

namespace tinygltf {
class Model;
}

namespace solis {
class SOLIS_CORE_API Model : public Object<Model>
{
public:
    Model(const string &file_name);
    ~Model();

    const vector<std::shared_ptr<Mesh>> &GetMeshes() const
    {
        return mMeshes;
    }

private:
    void LoadMeshes(tinygltf::Model &model);

private:
    vector<std::shared_ptr<Mesh>> mMeshes;
};
} // namespace solis