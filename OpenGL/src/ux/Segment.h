#pragma once

#include "Core.hpp"
#include "../Renderer.hpp"
#include "../Shader.hpp"
#include "../VertexBufferLayout.hpp"

namespace ux {


    class Segment
    {
    private:
        float* vertices;
        unsigned int* indicies;
        Ref<VertexArray> va;
        Ref<VertexBuffer> vb;
        Ref<VertexBufferLayout> layout;
        Ref<IndexBuffer> ib;
        // LINES
        Ref<VertexArray> vaLines;
        Ref<VertexBuffer> vbLines;
        Ref<VertexBufferLayout> layoutLines;
        Ref<IndexBuffer> ibLines;
    public:
        Segment(const float inner_radius, const float outer_radius, const float start_angle, const float end_angle, const float thickness);
        void Draw(const Renderer& renderer, const Shader& shader) const;
        //void DrawOutline(const Renderer& renderer, const Shader& shader) const;
        ~Segment();
    };

}