#pragma once

#include <glad/glad.h>

#include <iostream>

#include "VertexArray.hpp"
#include "IndexBuffer.hpp"
#include "Shader.hpp"




class Renderer
{
public:

    inline void Init() const
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_DEPTH_TEST);
    }

    inline void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) const
    {
        glViewport(x, y, width, height);
    }

    inline void SetClearColor(const glm::vec4& color) const
    {
        glClearColor(color.r, color.g, color.b, color.a);
    }

    inline void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const
    {
        Draw(GL_TRIANGLES, va, ib, shader);
    }

    inline void Draw(const unsigned int mode, const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const
    {
        shader.Bind();
        va.Bind();
        ib.Bind();
        glDrawElements(mode, ib.GetCount(), GL_UNSIGNED_INT, nullptr);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    inline void Clear() const
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
};