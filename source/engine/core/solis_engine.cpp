#include "core/solis_engine.hpp"

namespace solis
{
    static bool IsDestroyEngine = false;

    Engine::Engine(const EngineCreateInfo &info) : mCreateInfo(info)
    {
        if (sInstance != nullptr)
            throw std::runtime_error("Engine already exists.");

        sInstance = this;

        for (auto it = Module::Registry().begin(); it != Module::Registry().end(); ++it)
        {
            CreateModule(it, ModuleFilter());
        }

        // 初始化FileModule搜索路径

        // 这里将会调用GLFW， 所以并不清楚是否需要创建SwapChain
        // 这里代理了SwapChain，不需要手动释放，RenderModule会释放
        // mMainSwapChain = RenderModule::Get()->CreateSwapChain(param->mWindowInfo);
        // RenderModule::Get()->CreateRenderGraph(mMainSwapChain);

        // 下一帧触发
        // EVENT_SEND(EngineInitEvent, EngineInitEvent());
        // EVENT_POST(EngineInitPostEvent, EngineInitPostEvent());
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
        if (mModules.find(id.hash()) == mModules.end())
            return;

        // Destroy all module dependencies first.
        for (const auto &[registrarId, registrar] : Module::Registry())
        {
            if (std::find(registrar.require.begin(), registrar.require.end(), id) != registrar.require.end())
                DestroyModule(registrarId);
        }

        mModules.erase(id.hash());
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
}