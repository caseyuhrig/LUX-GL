#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "lux/Renderer/VertexArray.hpp"
#include "lux/Renderer/IndexBuffer.hpp"
#include "lux/Renderer/Shader.hpp"
#include "lux/Renderer/ShaderLibrary.hpp"
#include "lux/Renderer/Timer.hpp"


namespace lux {

    struct RendererData
    {
        //Ref<RenderPass> m_ActiveRenderPass;
        //RenderCommandQueue m_CommandQueue;
        ShaderLibrary m_ShaderLibrary;

        //Ref<VertexBuffer> m_FullscreenQuadVertexBuffer;
        //Ref<IndexBuffer> m_FullscreenQuadIndexBuffer;
        //Ref<Pipeline> m_FullscreenQuadPipeline;
        struct SceneInfo
        {
            glm::mat4 Projection;
            glm::mat4 View;
            glm::mat4 ViewProjection;
            glm::vec3 Position;
            //float pad1;
            glm::vec3 LookAt;
            float pad2;
            float zFar;
            float zNear;
            // shadow parameters
            bool Shadows;
            float ShadowBias;
            int ShadowSamples;
            
            // light parameters
            glm::vec3 LightPosition;
            float pad3;
            glm::vec3 LightColor;
        } SceneData;
        //struct SceneInfo
        //{
            //SceneRendererCamera SceneCamera;
        //    CameraSpecification SceneCamera;
        //    LightProperties ActiveLight; // TODO Creat Light not Lights class.

            // Resources
            //Ref<MaterialInstance> SkyboxMaterial;
            //Environment SceneEnvironment;
            //Light ActiveLight;
        //} SceneData;

        Timer timestep;
        float m_Gamma = 1.0f;
    };

    static RendererData s_Data;

    class Renderer
    {
    public:
        static void Init()
        {
            //s_Data.m_ShaderLibrary = ShaderLibrary::Create();
            s_Data.m_ShaderLibrary.Load("Base", "res/shaders/base.shader");
            s_Data.m_ShaderLibrary.Load("PointShadows", "res/shaders/point_shadows.glsl");
            s_Data.m_ShaderLibrary.Load("UX", "res/shaders/ux.shader");
            s_Data.m_ShaderLibrary.Load("Basic", "res/shaders/basic.shader");
        }

        static void SetViewport(const int32_t x, const int32_t y, const uint32_t width, const uint32_t height)
        {
            glViewport(x, y, width, height);
        }
        
        static void SetClearColor(const glm::vec4& color)
        {
            glClearColor(color.r, color.g, color.b, color.a);
        }
        
        static void Draw(const Ref<VertexArray>& va, const Ref<IndexBuffer>& ib, const Ref<Shader>& shader)
        {
            Draw(GL_TRIANGLES, va, ib, shader);
        }

        static void Draw(const uint32_t& mode, const Ref<VertexArray>& va, const Ref<IndexBuffer>& ib, const Ref<Shader>& shader)
        {
            shader->Bind();
            va->Bind();
            ib->Bind();
            glDrawElements(mode, ib->GetCount(), GL_UNSIGNED_INT, nullptr);
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        static void Clear() { glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); }
        static void ClearDepthBuffer() { glClear(GL_DEPTH_BUFFER_BIT); }
        static ShaderLibrary& ShaderLibrary() { return s_Data.m_ShaderLibrary; }
        static float& GetGamma() { return s_Data.m_Gamma; }
        static Timer& GetTimestep() { return s_Data.timestep; }
    };

}