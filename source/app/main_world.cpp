#include "main_world.hpp"

#include "core/data/game_object.hpp"

#include "core/world/component.hpp"

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

    GameObject *gameObject = new GameObject();

    auto transform    = gameObject->AddComponent<components::Transform>();
    auto meshRenderer = gameObject->AddComponent<components::MeshRenderer>();
    auto material     = gameObject->AddComponent<components::Material>();
}

void MainWorld::Update()
{
}