#pragma once

#include <string>


namespace lux
{
    class Layer
    {
    public:
        Layer(const std::string& debugName = "Layer");
        virtual ~Layer() = default;

        virtual void OnAttach() = 0;
        virtual void OnDetach() = 0;
        // needs a better name Render?
        virtual void OnUpdate() = 0;

        inline const std::string& GetName() const { return m_DebugName; }
    private:
        std::string m_DebugName;
    };
}