#include "core/solis_engine.hpp"

#include "core/graphics/graphics.hpp"
#include "core/events/events.hpp"
#include "core/events/event_define.hpp"
#include "core/world/world.hpp"

namespace solis {
static bool IsDestroyEngine = false;

Engine::Engine(const EngineCreateInfo &info) :
    mCreateInfo(info)
{
    if (sInstance != nullptr)
        throw std::runtime_error("Engine already exists.");

    sInstance = this;

    for (auto it = Module::Registry().begin(); it != Module::Registry().end(); ++it)
    {
        CreateModule(it, ModuleFilter());
    }

    // 初始化FileModule搜索路径
    using namespace graphics;
    Graphics::Get()->Init();
    mSwapchain = Graphics::Get()->CreateSurfaceSwapchain(info.window, info.windowSize);

    // 下一帧触发
    EVENT_SEND(EngineInitEvent());
    EVENT_POST(EngineInitPostEvent());
}

Engine::~Engine()
{
    Destroy();
}

void Engine::Destroy()
{
    if (IsDestroyEngine)
        return;

    // RenderModule::Get()->WaitRenderComplete();
    for (auto &[index, module] : mModules)
    {
        DestroyModule(index);
    }

    IsDestroyEngine = true;
    mSwapchain      = nullptr;
}

int Engine::Run()
{
    // 这里可能需要单开一个线程去跑这玩意
    return 0;
}

void Engine::CreateModule(Module::TRegistryMap::const_iterator it, const ModuleFilter &filter)
{
    if (mModules.find(it->first) != mModules.end())
        return;

    if (!filter.Check(it->first))
        return;

    // TODO: Prevent circular dependencies.
    for (auto requireId : it->second.require)
        CreateModule(Module::Registry().find(requireId), filter);

    auto &&module = it->second.create();
    mModules.emplace(it->first, std::move(module));
    mModuleStage[it->second.stage].emplace_back(it->first);
}

void Engine::DestroyModule(ctti::type_index id)
{
    auto it = mModules.find(id.hash());
    if (it == mModules.end())
        return;

    // Destroy all module dependencies first.
    for (const auto &[registrarId, registrar] : Module::Registry())
    {
        if (std::find(registrar.require.begin(), registrar.require.end(), id) != registrar.require.end())
            DestroyModule(registrarId);
    }

    it->second.reset();
    mModules.erase(it.getIndex());
}

void Engine::Step()
{
    UpdateStage(Module::Stage::Always);
    UpdateStage(Module::Stage::Pre);
    UpdateStage(Module::Stage::Normal);
    UpdateStage(Module::Stage::Post);
    UpdateStage(Module::Stage::Render);
}

void Engine::UpdateStage(Module::Stage stage)
{
    for (auto &typeIndex : mModuleStage[stage])
    {
        mModules.find(typeIndex.hash())->second->Update();
    }
}

void Engine::SetMainWorld(std::unique_ptr<WorldBase> &&world)
{
    World::Get()->SetMainWorld(world);
}
} // namespace solis