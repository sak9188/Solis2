#pragma once

#include "core/base/solis_core.hpp"
#include "core/base/object.hpp"
#include "core/base/using.hpp"

#include "core/graphics/buffer/buffer.hpp"

namespace solis
{
    struct Vertex
    {
        math::vec4 position;
        math::vec3 normal;
        math::vec2 texcoord;
    }

    struct VertexAttribute
    {
        VkFormat format = VK_FORMAT_UNDEFINED;
        uint32_t stride = 0;
        uint32_t offset = 0;
    };

    class Mesh : public Object<Mesh>
    {
        friend class Model;

    public:
        Mesh() = default;
        ~Mesh() = default;

        uint32_t VerticesCount() const { return mVerticesCount; }

        uint32_t IndicesCount() const { return mIndicesCount; }

        uint32_t IndexOffset() const { return mIndexOffset; }

        void SetAttribute(const string &name, const VertexAttribute &attribute)
        {
            mAttributes.insert({name, attribute});
        }

        bool GetAttribute(const string &name, VertexAttribute &attribute) const
        {
            auto it = mAttributes.find(name);
            if (it != mAttributes.end())
            {
                attribute = it->second;
                return true;
            }
            return false;
        }

    private:
        uint32_t mIndexOffset = 0;
        uint32_t mIndicesCount = 0;
        uint32_t mVerticesCount = 0;

        std::unique_ptr<graphics::Buffer> mIndexBuffer;

        hash_map<string, graphics::Buffer> mBuffers{1024};

        hash_map<string, VertexAttribute> mAttributes{MaxVertexAttributes};
    };
}