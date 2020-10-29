#pragma once

#include <vector>
#include "Layer.hpp"


namespace lux {

    class LayerStack
    {
    public:
        LayerStack() {}
        ~LayerStack() = default;

        void Attach(Layer* layer) 
        { 
            m_Layers.push_back(layer); 
            layer->OnAttach();
        }

        void OnUpdate() const
        {
            for (Layer* layer : m_Layers)
                layer->OnUpdate();
        }

        void RenderImGui()
        {
            for (Layer* layer : m_Layers)
                layer->OnImGuiRender();
        }

        std::vector<Layer*>::iterator begin() { return m_Layers.begin(); }
        std::vector<Layer*>::iterator end() { return m_Layers.end(); }
    private:
        std::vector<Layer*> m_Layers;
    };
}