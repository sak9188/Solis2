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
        };
    }
}
