#include "core/data/model.hpp"

namespace solis
{
    Model::Model(const string &file_name)
    {
        tinygltf::Model model;
        tinygltf::TinyGLTF loader;
        string err;
        string warn;

        bool ret = loader.LoadASCIIFromFile(&model, &err, &warn, file_name);

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

    void Model::LoadMeshes(tinygltf::Model &model)
    {
        for (auto &mesh : model.meshes)
        {
            auto m = std::make_shared<Mesh>();
            for (auto &primitive : mesh.primitives)
            {
                auto &attributes = primitive.attributes;
                auto &indices = primitive.indices;

                auto &position = attributes["POSITION"];
                auto &normal = attributes["NORMAL"];
                auto &texcoord = attributes["TEXCOORD_0"];

                auto &index = indices;

                auto &position_accessor = model.accessors[position];
                auto &normal_accessor = model.accessors[normal];
                auto &texcoord_accessor = model.accessors[texcoord];

                auto &index_accessor = model.accessors[index];

                auto &position_buffer_view = model.bufferViews[position_accessor.bufferView];
                auto &normal_buffer_view = model.bufferViews[normal_accessor.bufferView];
                auto &texcoord_buffer_view = model.bufferViews[texcoord_accessor.bufferView];

                auto &index_buffer_view = model.bufferViews[index_accessor.bufferView];

                auto &position_buffer = model.buffers[position_buffer_view.buffer];
                auto &normal_buffer = model.buffers[normal_buffer_view.buffer];
                auto &texcoord_buffer = model.buffers[texcoord_buffer_view.buffer];

                auto &index_buffer = model.buffers[index_buffer_view.buffer];

                auto position_data = reinterpret_cast<float *>(&position_buffer.data[position_buffer_view.byteOffset + position_accessor.byteOffset]);
                auto normal_data = reinterpret_cast<float *>(&normal_buffer.data[normal_buffer_view.byteOffset + normal_accessor.byteOffset]);
                auto texcoord_data = reinterpret_cast<float *>(&texcoord_buffer.data[texcoord_buffer_view.byteOffset + texcoord_accessor.byteOffset]);
                auto index_data = reinterpret_cast<uint32_t *>(&index_buffer.data[index_buffer_view.byteOffset + index_accessor.byteOffset]);

                m->SetAttribute("POSITION", {VK_FORMAT_R32G32B32_SFLOAT, sizeof(float) * 3, 0});
                m->SetAttribute("NORMAL", {VK_FORMAT_R32G32B32_SFLOAT, sizeof(float) * 3, 0});
                m->SetAttribute("TEXCOORD_0", {VK_FORMAT_R32G32_SFLOAT, sizeof(float) * 2, 0});

                m->mVerticesCount = position_accessor.count;
                m->mIndexOffset = index_accessor.count;

                vector<Vertex> vertices;
                for (size_t v = 0; v < m->mVerticesCount; v++)
                {
                    Vertex vertex{};
                    vertex.position = glm::vec4(glm::make_vec3(&position_data[v * 3]), 1.0f);
                    vertex.normal = glm::normalize(glm::make_vec3(&normal_data[v * 3]));
                    vertex.texcoord = glm::make_vec2(&texcoord_data[v * 2]);
                    vertices.push_back(vertex);
                }

                using BufferType = graphics::Buffer::Type;
                graphics::Buffer index_buffer(BufferType::Index, m->mIndexOffset * sizeof(uint32_t), index_data);
                graphics::Buffer vertex_buffer(BufferType::Vertex, m->mVerticesCount * sizeof(Vertex), vertices.data());

                m->mBuffers.insert({"vertex", std::move(vertex_buffer)});
                m->mIndexBuffer = std::move(index_buffer);

                mMeshes.push_back(m);
            }

            Log::SInfo("Load meshes: {}", mesh.name);
        }
    }
}