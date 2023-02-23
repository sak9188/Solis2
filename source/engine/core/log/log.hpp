#pragma once

#include "core/solis_core.hpp"
#include "core/base/object.hpp"

#include "fmt/core.h"

namespace solis
{
    class SOLIS_CORE_API Log : public Object<Log>
    {
    public:
        Log(string name = "") : mName(name){};
        virtual ~Log() = default;

        template <typename... Args>
        void Debug(const string &message, Args &&...args)
        {
            static const string debug = fmt::format("[{}][D]:{}\n", mName, message);
            fmt::print(debug.toStdString(), std::forward<Args>(args)...);
        };

        template <typename... Args>
        void Info(const string &message, Args &&...args)
        {
            static const string debug = fmt::format("[{}][I]:{}\n", mName, message);
            fmt::print(debug.toStdString(), std::forward<Args>(args)...);
        };

        template <typename... Args>
        void Warning(const string &message, Args &&...args)
        {
            static const string debug = fmt::format("[{}][W]:{}\n", mName, message);
            fmt::print(debug.toStdString(), std::forward<Args>(args)...);
        };

        template <typename... Args>
        void Error(const string &message, Args &&...args)
        {
            static const string debug = fmt::format("[{}][E]:{}\n", mName, message);
            fmt::print(debug.toStdString(), std::forward<Args>(args)...);
        };

        template <typename... Args>
        inline static void SDebug(const string &message, Args &&...args)
        {
            static Log logger("Static");
            logger.Debug(message, std::forward<Args>(args)...);
        };

        template <typename... Args>
        inline static void SInfo(const string &message, Args &&...args)
        {
            static Log logger("Static");
            logger.Info(message, std::forward<Args>(args)...);
        };

        template <typename... Args>
        inline static void SWarning(const string &message, Args &&...args)
        {
            static Log logger("Static");
            logger.Warning(message, std::forward<Args>(args)...);
        };

        template <typename... Args>
        inline static void SError(const string &message, Args &&...args)
        {
            static Log logger("Static");
            logger.Error(message, std::forward<Args>(args)...);
        };

    private:
        string mName;
    };
} // namespace solis
