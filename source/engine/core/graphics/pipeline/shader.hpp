#pragma once

#include "core/solis_core.hpp"
#include "core/base/object.hpp"
#include "core/base/using.hpp"

namespace soils
{
    namespace graphics
    {
        class SOLIS_CORE_API Shader : public Object<Shader>
        {
        public:
            Shader(const string &name, const string &path);
            virtual ~Shader();

            void CreateShaderModule(const vector<char> &code);

            const string &GetName() const { return mName; }
            const string &GetPath() const { return mPath; }
            const VkShaderModule &GetShaderModule() const { return mShaderModule; }

        private:
            string mName;
            string mPath;
            VkShaderModule mShaderModule;
        };
    }
}