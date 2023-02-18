#pragma once

#include "core/solis_core.h"

#include "core/base/using.h"
#include "core/base/object.h"

namespace solis
{
    namespace graphics
    {
        class SOLIS_CORE_API Graphics : public Object<Graphics>
        {
        public:
            Graphics() = default;
            virtual ~Graphics() = default;

            virtual void Init() = 0;
            virtual void Shutdown() = 0;
            virtual void Update() = 0;
            virtual void Render() = 0;
        };
    }
}
