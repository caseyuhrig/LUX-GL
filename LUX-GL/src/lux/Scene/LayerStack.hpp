#pragma once

#include <vector>
#include "Layer.hpp"


namespace lux {

    class LayerStack
    {
    public:
        LayerStack() {}

        ~LayerStack()
        {
            //for (Layer* layer : _layers)
            //{
                //delete layer;
            //}
        }

        void Add(Layer* layer)
        {
          
                _layers.push_back(layer);
            
        }
        void Draw() const
        {
            for (Layer* layer : _layers)
            {
                layer->Draw();
            }
        }

        void Begin()
        {
            for (Layer* layer : _layers)
            {
                layer->Begin();
            }
        }

        void End()
        {
            for (Layer* layer : _layers)
            {
                layer->End();
            }
        }
    private:
        std::vector<Layer*> _layers;
    };
}