#pragma once

#include <filesystem>
#include <iostream>

#include "core/solis_core.hpp"

#include "core/base/object.hpp"
#include "core/base/using.hpp"
#include "core/base/module.hpp"

#include "core/files/file_info.hpp"

namespace solis {
namespace files {
namespace fs = std::filesystem;
class SOLIS_CORE_API Files : public Object<Files>, public Module::Registrar<Files>
{
    inline static const bool Registered = Register(Stage::Never);

public:
    Files()
    {
        // 设置当前路径为可执行文件路径
        Files::Get()->SetCurrentPath(Files::Get()->GetExecuteBinPath());
    };
    virtual ~Files() = default;

    virtual void Update() override
    {
    }

    string GetCurrentPath() const
    {
        return fs::current_path().string();
    }

    void SetCurrentPath(const string &path) const
    {
        fs::current_path(path.toStdString());
    }

    vector<string> ReadTextFile(const string &path)
    {
        files::FileInfo fileInfo(path);
        if (!fileInfo.Exist())
        {
            return {};
        }
        return fileInfo.Read();
    }

    vector<uint8_t> ReadFile(const string &path)
    {
        files::FileInfo fileInfo(path);
        if (!fileInfo.Exist())
        {
            return {};
        }
        return fileInfo.ReadBytes();
    }

    string GetExecuteBinPath()
    {
#ifdef __WIN__
        char buffer[MAX_PATH];
        GetModuleFileName(NULL, (LPSTR)buffer, MAX_PATH);
        fs::path full_path(buffer);
        return full_path.parent_path().string();
#elif __LINUX__
        char    buffer[PATH_MAX];
        ssize_t len = ::readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
        if (len != -1)
        {
            buffer[len] = '\0';
            fs::path full_path(buffer);
            return full_path.parent_path().string();
        }
        else
        {
            return "";
        }
#endif
    }
};
}
} // namespace solis::files