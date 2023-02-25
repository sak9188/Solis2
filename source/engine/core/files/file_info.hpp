#pragma once

#include "core/solis_core.hpp"

#include "core/base/object.hpp"
#include "core/base/using.hpp"
#include "core/base/module.hpp"

#include <filesystem>

namespace solis
{
    namespace files
    {
        namespace fs = std::filesystem;

        enum class FileWriteMode
        {
            Append,
            Overwrite
        };

        class SOLIS_CORE_API FileInfo : public Object<FileInfo>
        {
        public:
            // 这里是相对路径
            FileInfo(const string &path);
            virtual ~FileInfo();

            // GetPath()
            const string GetPath() const { return mPath.string(); }

            // GetDirectory()
            const string GetDirectory() const { return mPath.parent_path().string(); }

            // GetFilename()
            const string GetFilename() const { return mPath.filename().string(); }

            // GetExtension()
            const string GetExtension() const { return mPath.extension().string(); }

            // IsExist()
            bool IsExist() const;

            // Read()
            vector<string> Read() const;

            // ReadBytes()
            vector<uint8_t> ReadBytes() const;

            // Write()
            void Write(const vector<string> &lines, FileWriteMode mode = FileWriteMode::Overwrite) const;

            // WriteBytes()
            void WriteBytes(const vector<uint8_t> &bytes, FileWriteMode mode = FileWriteMode::Overwrite) const;

        private:
            fs::path mPath;
        };
    }
} // namespace solis
