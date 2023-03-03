#pragma once

#include "core/solis_core.hpp"

namespace solis {
class SOLIS_CORE_API INonCopyable
{
public:
    INonCopyable()          = default;
    virtual ~INonCopyable() = default;

    // Copy Constructor
    INonCopyable(const INonCopyable &) = delete;

    // Copy Assignment Operator
    INonCopyable &operator=(const INonCopyable &) = delete;
};
} // namespace solis