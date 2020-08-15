#pragma once

#include <string>


namespace lux
{
    class Layer
    {
    public:
        Layer(const std::string& debugName = "Layer");
        ~Layer() {}
        virtual void Begin() = 0;
        virtual void End() = 0;
        // needs a better name Render?
        virtual void Draw() const = 0;
        inline const std::string& GetName() const { return m_DebugName; }
    private:
        std::string m_DebugName;
    };
}