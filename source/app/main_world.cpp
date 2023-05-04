#include "main_world.hpp"

#include "core/data/game_object.hpp"

#include "core/world/component.hpp"
#include "core/world/system.hpp"

#include "core/assets/assets.hpp"
#include "core/data/model.hpp"
#include "core/data/texture.hpp"

using namespace solis;

void MainWorld::Start()
{
    using namespace assets;
    // Create GameObject
    /**
     * GameObject* gameObject = new GameObject();
     * gameObject->AddComponent(new Transform());
     * gameObject->AddComponent(new MeshRenderer());
     *
     * GameObject* Camera = new GameObject();
     * Camera->AddComponent(new Transform());
     * Camera->AddComponent(new Camera());
     *
     * GameObject* Light = new GameObject();
     * Light->AddComponent(new Transform());
     * Light->AddComponent(new Light());
     */

    // TODO: 这里没有释放内存
    GameObject *gameObject = new GameObject();

    // PhysicsSystem::Get()->AllocRigidBody();

    auto transfrom = TransformSystem::Get()->AllocTransform();
    gameObject->AddComponent(transfrom);

    // CameraSystem::Get()->AllocCamera();

    // RenderSystem 只接受主动提交过来的数据
    // 不接受监视创建的数据
    // auto meshRenderer = RenderSystem::Get()->AllocRenderable();

    // TODO: AssetBundle = LoadAssetBundle("filePath", bool 是否挂载在Files里, uint32_t pri 载入优先级) 设置资产包，然后在资产包中加载资源

    // 载入模型资源
    auto cube     = Assets::Get()->LoadObject<Model>("gltfs/cube/cube.gltf");
    auto cubeData = cube->Data<Model>();
    auto meshCom  = components::Mesh::Get();
    meshCom->SetMeshs(cubeData->GetMeshes());
    gameObject->AddComponent(meshCom);

    // 载入材质资源
    // std::unique_ptr<Asset> asset2 = Asset::Get()->LoadObject<graphics::Material>("assets/material/");
    auto material     = std::make_shared<graphics::Material>("shaders/sponza");
    auto meshRenderer = components::MeshRenderer::Get();
    meshRenderer->SetMaterials({material});

    auto texture = std::make_shared<Texture>("gltfs/cube/swap.jpg");
    material->SetTexture("texSampler", texture);
}

void MainWorld::Update()
{
}