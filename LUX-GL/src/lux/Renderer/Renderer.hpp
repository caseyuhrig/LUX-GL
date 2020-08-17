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
        }

        ~Renderer() = default;

        void Init() const
        {
            glDisable(GL_BLEND);
            //glEnable(GL_BLEND);
            //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            //glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            glEnable(GL_DEPTH_TEST);

            //glDisable(GL_CULL_FACE);
            glEnable(GL_CULL_FACE);
            //glCullFace(GL_BACK);
            //glFrontFace(GL_CCW);
            //glEnable(GL_DEPTH_TEST);
            //glDepthFunc(GL_LESS);
            //glEnable(GL_MULTISAMPLE);
            //glEnable(GL_LINE_SMOOTH);
            //glEnable(GL_BLEND);
            //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            //glDisable(GL_CULL_FACE);

                /*
        // TODO put in the renderer init?
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        //glLineWidth(2.5f);
        glPointSize(5.0);
        glEnable(GL_MULTISAMPLE);
        //glDisable(GL_MULTISAMPLE);
        glEnable(GL_LINE_SMOOTH);
        // Enable depth test
        glEnable(GL_DEPTH_TEST);
        // Accept fragment if it closer to the camera than the former one
        glDepthFunc(GL_LESS);
        // Make sure to turn backface culling off (glDisable(GL_CULL_FACE) ) because since we can
        // look through the mesh, we could see that it has no “back” face.
        glDisable(GL_CULL_FACE);
        //glEnable(GL_FRAMEBUFFER_SRGB);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        */

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

        void Draw(const unsigned int mode, const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const
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
            // not sure why I am having to set these for each frame.
            // guessing something is messing with the depth buffer and not setting it back.
            // Canvas needs checked.
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);
        }

        float GetTime() { return static_cast<float>(glfwGetTime()); }
        float& Gamma() { return gamma; }

    private:
        float gamma = 1.0;
    };

}