#include "core/files/file_info.hpp"

#include <fstream>

namespace solis
{
    namespace files
    {
        FileInfo::FileInfo(const string &path) : mPath(path.toStdString())
        {
        }

        FileInfo::~FileInfo()
        {
        }

        bool FileInfo::Exist() const
        {
            return std::filesystem::exists(mPath);
        }

        vector<string> FileInfo::Read() const
        {
            // Read From File
            std::ifstream file(mPath);
            vector<string> lines;
            std::string line;
            while (std::getline(file, line))
            {
                lines.push_back(line);
            }
            return lines;
        }

        vector<uint8_t> FileInfo::ReadBytes() const
        {
            std::ifstream file(mPath, std::ios::in | std::ios::binary);
            file.seekg(0, std::ios::end);
            size_t fileSize = (size_t)file.tellg();
            vector<uint8_t> buffer(fileSize);
            file.seekg(0);
            file.read((char *)buffer.data(), fileSize);
            return buffer;
        }

        void FileInfo::Write(const vector<string> &lines, FileWriteMode mode) const
        {
            std::ofstream file(mPath, mode == FileWriteMode::Append ? std::ios::app : std::ios::out);
            for (const auto &line : lines)
            {
                file << line << std::endl;
            }
        }

        void FileInfo::WriteBytes(const vector<uint8_t> &bytes, FileWriteMode mode) const
        {
            std::ofstream file(mPath, mode == FileWriteMode::Append ? std::ios::app : std::ios::out | std::ios::binary);
            file.write((char *)bytes.data(), bytes.size());
        }
    }
}