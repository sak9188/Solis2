#pragma once

#include "core/solis_core.hpp"
#include "core/base/object.hpp"
#include "core/base/using.hpp"

#include <iostream>
#include <chrono>
#include <iomanip>
#include <ctime>

namespace solis {
namespace os {
class SOLIS_CORE_API Chrono : public Object<Chrono>
{
public:
    inline static string GetNowChronoFormat(const string &format = "%Y-%m-%d %H:%M:%S")
    {
        auto now       = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);

        std::stringstream ss;
        ss << std::put_time(std::localtime(&in_time_t), format.c_str());
        return ss.str();
    }
};
}
} // namespace solis::os