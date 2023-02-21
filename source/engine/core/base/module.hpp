#pragma once

#include <bitset>
#include <unordered_map>
#include <memory>
#include <functional>

#include "core/solis_core.hpp"
#include "core/base/using.hpp"

#include "ctti/type_id.hpp"

#include "core/base/i_noncopyable.hpp"

namespace solis
{
    // 最大的模块数量
    inline static size_t const MaxModules = 32;

    using TypeId = uint64_t;

    // operator uint64 for TypeId
    // inline uint64_t operator()(const TypeId &val)
    // {
    //     return val.hash();
    // }

    template <typename Base>
    class ModuleFactory
    {
    public:
        class TCreateValue
        {
        public:
            std::function<std::unique_ptr<Base>()> create;
            typename Base::Stage stage;
            vector<TypeId> require;
        };

        using TRegistryMap = hash_map<TypeId, TCreateValue>;

        virtual ~ModuleFactory() = default;

        static TRegistryMap &Registry()
        {
            static TRegistryMap impl{MaxModules};
            return impl;
        }

        template <typename... Args>
        class Requires
        {
        public:
            vector<TypeId> Get() const
            {
                vector<TypeId> require;
                (require.emplace_back(ctti::type_id<Args>().hash()), ...);
                return require;
            }
        };

        template <typename T>
        class Registrar : public Base
        {
        public:
            virtual ~Registrar()
            {
                if (static_cast<T *>(this) == moduleInstance)
                    moduleInstance = nullptr;
            }

            static T *Get() { return moduleInstance; }

        protected:
            template <typename... Args>
            static bool Register(typename Base::Stage stage, Requires<Args...> &&require = {})
            {
                ModuleFactory::Registry().insert(
                    {ctti::type_id<T>().hash(),
                     {[]()
                      {
                          moduleInstance = new T();
                          // The registrar does not own the instance, the engine does, we just hold a raw pointer for convenience.
                          return std::unique_ptr<Base>(moduleInstance);
                      },
                      stage, require.Get()}});
                return true;
            }

            inline static T *moduleInstance = nullptr;
        };
    };

    class SOLIS_CORE_API Module : public ModuleFactory<Module>, INonCopyable
    {
    public:
        enum class Stage : uint8_t
        {
            Never,
            Always,
            Pre,
            Normal,
            Post,
            Render
        };

        using StageIndex = std::pair<Stage, TypeId>;

        virtual ~Module() = default;

        virtual void Update() = 0;
    };

    // template class SOLIS_CORE_API TypeInfo<Module>;

    class SOLIS_CORE_API ModuleFilter
    {
    public:
        ModuleFilter()
        {
            include.set();
        }

        template <typename T>
        bool Check() const noexcept
        {
            return include.test(ctti::type_id<T>().hash());
        }

        bool Check(TypeId typeId) const noexcept
        {
            return include.test(typeId);
        }

        template <typename T>
        ModuleFilter &Exclude() noexcept
        {
            include.reset(ctti::type_id<T>().hash());
            return *this;
        }

        template <typename T>
        ModuleFilter &Include() noexcept
        {
            include.set(ctti::type_id<T>().hash());
            return *this;
        }

        ModuleFilter &ExcludeAll() noexcept
        {
            include.reset();
            return *this;
        }

        ModuleFilter &IncludeAll() noexcept
        {
            include.set();
            return *this;
        }

    private:
        std::bitset<64> include;
    };
}
