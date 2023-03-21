#pragma once

#include "core/solis_core.hpp"
#include "core/base/object.hpp"

#include "core/os/chrono.hpp"

#include "fmt/core.h"
#include "fmt/color.h"

namespace solis {
class SOLIS_CORE_API Log : public Object<Log>
{
public:
    Log(const string &name = "") :
        mName(name){};
    Log(const string &name, const fmt::text_style &textStyle) :
        mName(name), mTextStyle(textStyle){};
    virtual ~Log() = default;

    template <typename... Args>
    void Debug(const string &message, Args &&...args)
    {
        const string debug = fmt::format("[{}][{}][D]:{}\n", os::Chrono::GetNowChronoFormat(), mName, message);
        fmt::print(mTextStyle, debug.c_str(), std::forward<Args>(args)...);
    };

    template <typename... Args>
    void Info(const string &message, Args &&...args)
    {
        const string debug = fmt::format("[{}][{}][I]:{}\n", os::Chrono::GetNowChronoFormat(), mName, message);
        fmt::print(mTextStyle, debug.c_str(), std::forward<Args>(args)...);
    };

    template <typename... Args>
    void Warning(const string &message, Args &&...args)
    {
        const string debug = fmt::format("[{}][{}][W]:{}\n", os::Chrono::GetNowChronoFormat(), mName, message);
        fmt::print(mTextStyle, debug.c_str(), std::forward<Args>(args)...);
    };

    template <typename... Args>
    void Error(const string &message, Args &&...args)
    {
        const string debug = fmt::format("[{}][{}][E]:{}\n", os::Chrono::GetNowChronoFormat(), mName, message);
        fmt::print(mTextStyle, debug.c_str(), std::forward<Args>(args)...);
    };

    template <typename... Args>
    inline static void SDebug(const string &message, Args &&...args)
    {
        static Log logger("Static", fg(fmt::color::blue));
        logger.Debug(message, std::forward<Args>(args)...);
    };

    template <typename... Args>
    inline static void SInfo(const string &message, Args &&...args)
    {
        static Log logger("Static", fg(fmt::color::white_smoke));
        logger.Info(message, std::forward<Args>(args)...);
    };

    template <typename... Args>
    inline static void SWarning(const string &message, Args &&...args)
    {
        static Log logger("Static", fg(fmt::color::yellow) | fmt::emphasis::italic);
        logger.Warning(message, std::forward<Args>(args)...);
    };

    template <typename... Args>
    inline static void SError(const string &message, Args &&...args)
    {
        static Log logger("Static", fg(fmt::color::red) | bg(fmt::color::black) | fmt::emphasis::bold);
        logger.Error(message, std::forward<Args>(args)...);
    };

private:
    string          mName;
    fmt::text_style mTextStyle;
};
} // namespace solis
