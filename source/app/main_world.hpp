#pragma once

#include "core/solis_core.hpp"
#include "core/base/object.hpp"
#include "core/base/using.hpp"

#include "core/world/world_base.hpp"

class MainWorld : public Object<MainWorld>, public solis::WorldBase
{
public:
    OBJECT_NEW_DELETE(MainWorld)
    MainWorld()          = default;
    virtual ~MainWorld() = default;

    virtual void Start() override;
    virtual void Update() override;
};
