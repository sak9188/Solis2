#include "main_world.hpp"

#include "core/data/game_object.hpp"

#include "core/world/component.hpp"
#include "core/world/system.hpp"

#include "core/assets/assets.hpp"
#include "core/data/model.hpp"

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

    // 载入模型资源
    auto cube         = Assets::Get()->LoadObject<Model>("gltfs/cube/cube.gltf");
    auto cubeData     = cube->Data<Model>();
    auto meshRenderer = components::Mesh::Get();
    meshRenderer->SetMeshs(cubeData->GetMeshs());
    gameObject->AddComponent(meshRenderer);

    auto cube         = Assets::Get()->LoadObject<Model>("gltfs/cube/cube.gltf");
    auto cubeData     = cube->Data<Model>();
    auto meshRenderer = components::Mesh::Get();
    meshRenderer->SetMeshs(cubeData->GetMeshs());
    gameObject->AddComponent(meshRenderer);
    // std::unique_ptr<Asset> asset2 = AssetsSystem::Get()->Load("assets/material/");
    // std::unique_ptr<Material> material = std::make_unique<Material>("shaders/sponza");
    // MeshRenderer->SetMaterial(material);
}

void MainWorld::Update()
{
}