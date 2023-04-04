#pragma once

#include "core/solis_core.hpp"

#include "core/base/object.hpp"
#include "core/base/i_noncopyable.hpp"
#include "core/base/i_destroyable.hpp"

#include "core/base/module.hpp"

#include "ctti/type_id.hpp"

namespace solis {

class WorldBase;
struct SOLIS_CORE_API EngineCreateInfo
{
    math::vec2 windowSize;

    void *window = nullptr;

    // Vulkan扩展
    vector<const char *> extensions;
};

namespace graphics {
class Swapchain;
}

class SOLIS_CORE_API Engine : public Object<Engine>, public INonCopyable, public IDestroyable
{
public:
    Engine(const EngineCreateInfo &info);

    virtual ~Engine();

    void Destroy();

    virtual int Run();

    virtual void Step();

    // virtual void Update();

    inline static Engine *Get()
    {
        return sInstance;
    }

    const EngineCreateInfo &CreateInfo() const
    {
        return mCreateInfo;
    }

    // TODO: 临时
    graphics::Swapchain &GetSwapchain() const
    {
        return *mSwapchain;
    };

    void SetMainWorld(std::unique_ptr<WorldBase> &&world);

private:
    bool ParseEngineCfg(const string &filename);

    void InitFileModuleSearchPath();

    void CreateModule(Module::TRegistryMap::const_iterator it, const ModuleFilter &filter);

    void DestroyModule(ctti::type_index id);

    void UpdateStage(Module::Stage stage);

    hash_map<uint64_t, std::unique_ptr<Module>> mModules{MaxModules};

    std::map<Module::Stage, vector<ctti::type_index>> mModuleStage;

private:
    inline static Engine *sInstance = nullptr;

    // TODO: 临时
    graphics::Swapchain *mSwapchain = nullptr;

    const EngineCreateInfo mCreateInfo;
};
} // namespace solis