#pragma once

#include "core/solis_core.hpp"
#include "core/base/object.hpp"

namespace solis
{
    struct SOLIS_CORE_API EngineCreateInfo
    {
        // 扩展
        vector<const char *> extensions;
    };

    class SOLIS_CORE_API Engine : public Object<Engine>
    {
    public:
        Engine(const EngineCreateInfo &info) : mCreateInfo(info)
        {
            if (sInstance != nullptr)
                throw std::runtime_error("Engine already exists.");

            sInstance = this;
        };
        virtual ~Engine() = default;

        inline static Engine *Get()
        {
            return sInstance;
        }

        const EngineCreateInfo &CreateInfo() const { return mCreateInfo; }

    private:
        inline static Engine *sInstance = nullptr;

        const EngineCreateInfo mCreateInfo;
    };
}