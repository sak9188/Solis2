#pragma once

#include "core/solis_core.h"

namespace solis { namespace core {
    
    class SOLIS_CORE_ABI ObjectBase
    {
    public:
        ObjectBase() = default;
        virtual ~ObjectBase() = default;

        static void* Malloc(size_t size, );
    };

    template <typename T>
    class Object : public ObjectBase
    {

    };
    
}}