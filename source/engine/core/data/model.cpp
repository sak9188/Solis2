#include "core/data/model.hpp"
#include "core/log/log.hpp"

#include "core/math/math.hpp"

#ifndef TINYGLTF_IMPLEMENTATION
#define TINYGLTF_IMPLEMENTATION
#endif

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif

#ifndef STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#endif

#ifndef TINYGLTF_NOEXCEPTION
#define TINYGLTF_NOEXCEPTION
#endif

#ifndef JSON_NOEXCEPTION
#define JSON_NOEXCEPTION
#endif

#include "tiny_gltf.h"

namespace solis {
Model::Model(const string &file_name)
{
    tinygltf::Model    model;
    tinygltf::TinyGLTF loader;
    std::string        err;
    std::string        warn;

    bool ret = loader.LoadASCIIFromFile(&model, &err, &warn, file_name.toStdString());

    if (!warn.empty())
    {
        Log::SWarning("{}", warn);
    }

    if (!err.empty())
    {
        Log::SError("{}", err);
    }

    if (!ret)
    {
        Log::SError("Failed to load glTF: {}", file_name);
    }
    else
    {
        Log::SInfo("Load glTF: {}", file_name);

        LoadMeshes(model);
    }
}

Model::~Model(){};

std::vector<uint8_t> convert_underlying_data_stride(const std::vector<uint8_t> &src_data, uint32_t src_stride, uint32_t dst_stride)
{
    auto elem_count = uint32_t(src_data.size()) / src_stride;

    std::vector<uint8_t> result(elem_count * dst_stride);

    for (uint32_t idxSrc = 0, idxDst = 0;
         idxSrc < src_data.size() && idxDst < result.size();
         idxSrc += src_stride, idxDst += dst_stride)
    {
        std::copy(src_data.begin() + idxSrc, src_data.begin() + idxSrc + src_stride, result.begin() + idxDst);
    }

    return result;
}

void Model::LoadMeshes(tinygltf::Model &model)
{
    for (auto &mesh : model.meshes)
    {
        auto m = std::make_shared<Mesh>();
        for (auto &primitive : mesh.primitives)
        {
            auto &attributes = primitive.attributes;
            auto &indices    = primitive.indices;

            auto &position = attributes["POSITION"];
            auto &normal   = attributes["NORMAL"];
            auto &texcoord = attributes["TEXCOORD_0"];

            auto &index = indices;

            auto &position_accessor = model.accessors[position];
            auto &normal_accessor   = model.accessors[normal];
            auto &texcoord_accessor = model.accessors[texcoord];

            auto &index_accessor = model.accessors[index];

            auto &position_buffer_view = model.bufferViews[position_accessor.bufferView];
            auto &normal_buffer_view   = model.bufferViews[normal_accessor.bufferView];
            auto &texcoord_buffer_view = model.bufferViews[texcoord_accessor.bufferView];

            auto &index_buffer_view = model.bufferViews[index_accessor.bufferView];

            auto &position_buffer = model.buffers[position_buffer_view.buffer];
            auto &normal_buffer   = model.buffers[normal_buffer_view.buffer];
            auto &texcoord_buffer = model.buffers[texcoord_buffer_view.buffer];

            auto &index_buffer = model.buffers[index_buffer_view.buffer];

            auto position_data = reinterpret_cast<float *>(&position_buffer.data[position_buffer_view.byteOffset + position_accessor.byteOffset]);
            auto normal_data   = reinterpret_cast<float *>(&normal_buffer.data[normal_buffer_view.byteOffset + normal_accessor.byteOffset]);
            auto texcoord_data = reinterpret_cast<float *>(&texcoord_buffer.data[texcoord_buffer_view.byteOffset + texcoord_accessor.byteOffset]);

            uint32_t sizeIndex = 0;
            switch (index_accessor.componentType)
            {
            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE: {
                sizeIndex = 1;
                break;
            }
            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT: {
                sizeIndex = 2;
                break;
            }
            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT: {
                sizeIndex = 4;
                break;
            }
            default:
                break;
            }
            auto index_data = reinterpret_cast<uint8_t *>(&index_buffer.data[index_buffer_view.byteOffset + index_accessor.byteOffset]);

            m->SetAttribute("POSITION", {VK_FORMAT_R32G32B32_SFLOAT, sizeof(float) * 4, 0});
            m->SetAttribute("NORMAL", {VK_FORMAT_R32G32B32_SFLOAT, sizeof(float) * 3, sizeof(float) * 3});
            m->SetAttribute("TEXCOORD_0", {VK_FORMAT_R32G32_SFLOAT, sizeof(float) * 2, sizeof(float) * 7});

            m->mVerticesCount = position_accessor.count;
            m->mIndicesCount  = index_accessor.count;

            vector<Vertex> vertices;
            for (size_t v = 0; v < m->mVerticesCount; v++)
            {
                Vertex vertex{};
                vertex.position = math::vec4(math::make_vec3(&position_data[v * 3]), 1.0f);
                vertex.normal   = math::normalize(math::make_vec3(&normal_data[v * 3]));
                vertex.texcoord = math::make_vec2(&texcoord_data[v * 2]);
                vertices.push_back(vertex);
            }

            using BufferType = graphics::Buffer::Type;
            graphics::Buffer vertexBuffer{BufferType::Vertex, m->mVerticesCount * sizeof(Vertex), vertices.data()};
            m->mBuffers.insert({"vertex", std::move(vertexBuffer)});

            auto indices = convert_underlying_data_stride(byteData, sizeIndex, 4);

            m->mIndexBuffer = std::make_unique<graphics::Buffer>(BufferType::Index, m->mIndicesCount * 4, (void *)indices.data());

            mMeshes.push_back(m);
        }

        Log::SInfo("Load meshes: {}", mesh.name);
    }
}
} // namespace solis