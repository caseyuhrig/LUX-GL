#pragma once

#include <string>


namespace lux
{
    class Layer
    {
    public:
        Layer(const uint32_t width, const uint32_t height);
        ~Layer() {}
        virtual void Begin() = 0;
        virtual void End() = 0;
        // needs a better name Render?
        virtual void Draw() const = 0;
        virtual void Resize(const uint32_t width, const uint32_t height) = 0;
        inline const std::string& GetName() const { return m_DebugName; }
        void SetDebugLabel(const std::string_view label) { m_DebugName = label; }
    protected:
        std::string m_DebugName;
        uint32_t m_Width;
        uint32_t m_Height;
    };
}