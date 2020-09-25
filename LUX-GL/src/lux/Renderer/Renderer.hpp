#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "VertexArray.hpp"
#include "IndexBuffer.hpp"
#include "Shader.hpp"


namespace lux {

    class Renderer
    {
    public:
        Renderer() 
        {
            SetClearColor(glm::vec4(0, 0, 0, 1));
            //Init();
        }
        ~Renderer() = default;

        void Init() const
        {
            glDisable(GL_BLEND);
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_CULL_FACE);
        }

        void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) const
        {
            glViewport(x, y, width, height);
        }
        
        void SetClearColor(const glm::vec4& color) const
        {
            glClearColor(color.r, color.g, color.b, color.a);
        }
        
        void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const
        {
            Draw(GL_TRIANGLES, va, ib, shader);
        }

        void Draw(const unsigned int& mode, const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const
        {
            shader.Bind();
            va.Bind();
            ib.Bind();
            glDrawElements(mode, ib.GetCount(), GL_UNSIGNED_INT, nullptr);
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        void Clear() const
        {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }

        float& Gamma() { return gamma; }

    private:
        float gamma = 1.0;
    };

}