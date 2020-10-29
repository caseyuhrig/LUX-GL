#pragma once

#include <unordered_map>
#include "lux/Renderer/Shader.hpp"


namespace lux {

    class ShaderLibrary
    {
    public:
        static Ref<ShaderLibrary> Create() { return CreateRef<ShaderLibrary>(); }

        const Ref<Shader>& Get(const std::string& name)
        {
            if (!m_Shaders.contains(name))
            {
                int n = 0;
                std::string message = "Shader '" + name + "' not found!... Available shaders: ";
                for (const auto& element : m_Shaders)
                {
                    if (n > 0) message += ", ";
                    message += element.first;
                    n++;
                }
                UX_LOG_FATAL("%s", message);
                throw std::exception(message.c_str());
                //return nullptr;
                //assert(false);
            }
            return m_Shaders.at(name);
        }

        void Load(const std::string& name, const std::string& filepath)
        {
            auto shader = Shader::Create(filepath);
            m_Shaders.emplace(name, shader);
        }
    private:
        std::unordered_map<std::string, Ref<Shader>> m_Shaders;
    };
}