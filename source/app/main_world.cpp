#include "main_world.hpp"

#include "core/data/game_object.hpp"

#include "core/world/component.hpp"
#include "core/world/system.hpp"

#include "core/assets/assets.hpp"

using namespace solis;

void MainWorld::Start()
{
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

    auto meshRenderer = RenderSystem::Get()->AllocRenderable();
    gameObject->AddComponent(meshRenderer);

    // 载入模型资源
    std::unique_ptr<Model> asset = Assets::Get()->Load<Model>("gltfs/cube/cube.gltf");
    meshRenderer->SetMesh(asset->Get<Mesh>());

    // std::unique_ptr<Asset> asset2 = AssetsSystem::Get()->Load("assets/material/");
    std::unique_ptr<Material> material = std::make_unique<Material>("shaders/sponza");
    MeshRenderer->SetMaterial(material);
}

void MainWorld::Update()
{
}