#pragma once

#include <string>


namespace lux
{
    class Layer
    {
    public:
        Layer(const std::string& label = "Untitled layer")
            : m_Label(label)
        {}
        ~Layer() = default;
        virtual void OnAttach() {};
        virtual void OnDetach() {};
        virtual void OnUpdate() {};
        virtual void OnImGuiRender() {};
        const std::string& GetLabel() const { return m_Label; }
        void SetLabel(const std::string_view label) { m_Label = label; }
    protected:
        std::string m_Label;
    };
}