#pragma once

#include <gl/glew.h>

#include "glm/gtc/matrix_transform.hpp"

#include "lux/Primitive/Mesh.hpp"
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
        //struct SceneInfo {} SceneData;

        struct CameraInfo               // binding_point = 10
        {
            glm::mat4 Projection;       //  64    0
            glm::mat4 View;             //  64   64
            glm::mat4 ViewProjection;   //  64  128
            glm::vec3 Position;         //  16  192
            float p1 = 0;
            glm::vec3 LookAt;           //  16  208
            //float p2 = 0;
            float zFar = 2000.0f;       //   4  224
            float zNear = 0.01f;        //   4  228
            float AspectRatio;          //   4  232
            float Angle = 55.0f;        //   4  236
            glm::vec3 Up = { 0, 1, 0 }; //  16  248
            uint32_t ViewportWidth;     //   4  264
            uint32_t ViewportHeight;    //   4  268

            CameraInfo() : Position(0.0f, 0.0f, -100.0f), LookAt(glm::vec3(0.0f)), AspectRatio(1.0f) {}

            void Resize(const uint32_t width, const uint32_t height)
            {
                ViewportWidth = width;
                ViewportHeight = height;
                AspectRatio = static_cast<float>(width) / static_cast<float>(height);
                Projection = glm::perspective(glm::radians(Angle), AspectRatio, zNear, zFar);
                View = glm::lookAt(Position, LookAt, Up);
                ViewProjection = Projection * View;
            }

            void Update()
            {
                Projection = glm::perspective(glm::radians(Angle), AspectRatio, zNear, zFar);
                View = glm::lookAt(Position, LookAt, Up);
                ViewProjection = Projection * View;
            }
        } CameraData;                 //>272< 272

        struct ShadowInfo // 11
        {
            // shadow parameters
            float ShadowBias;         //  4
            int ShadowSamples;        //  4
            float ShadowAmount;       //  4
            bool Shadows;             //  1
            bool a;
            float b, c;
        } ShadowData;                 //>16< 7

        struct LightInfo // 12
        {
            // light parameters
            glm::vec3 LightPosition;  // 16   0
            float p1 = 0;
            glm::vec4 LightColor;     // 16  16
            //float p2 = 0;
        } LightData;                //>32< 32

        struct MaterialSpecification
        {
            glm::vec4 Color;   // 16   0
            glm::vec4 Ambient = glm::vec4(glm::vec3(0.3f), 1.0f); //vec3(1.0f, 0.5f, 0.31f); // 16  16
            glm::vec4 Diffuse = glm::vec4(1.0f); //vec3(1.0f, 0.5f, 0.31f);           // 16  32
            glm::vec4 Specular = glm::vec4(1.0f); //vec3(0.5f, 0.5f, 0.5f);           // 16  48
            float Shininess = 64.0; // [32.0f]    // 16 64
        } MaterialData;               // 4  68  >80<
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

        //struct TransformsInfo // 13
        //{
        //    glm::mat4 Transform[10];
        //} TransformsData;

        Timer timestep;
        float m_Gamma = 1.0f;
    };

    class DynaBuffer
    {
    public:
    private:
    };

    static RendererData s_Data;

    class Renderer
    {
    public:
        static void Init(const uint32_t width, const uint32_t height)
        {
            //s_Data.m_ShaderLibrary = ShaderLibrary::Create();
            s_Data.m_ShaderLibrary.Load("Base", "res/shaders/base.shader");
            s_Data.m_ShaderLibrary.Load("PointShadows", "res/shaders/point_shadows.glsl");
            s_Data.m_ShaderLibrary.Load("UX", "res/shaders/ux.shader");
            s_Data.m_ShaderLibrary.Load("Basic", "res/shaders/basic.shader");

            s_Data.CameraData.Up = { 0.0f, 1.0f, 0.0f };
            s_Data.CameraData.Angle = 55.0f;
            s_Data.CameraData.ViewportWidth = width;
            s_Data.CameraData.ViewportHeight = height;
            s_Data.CameraData.AspectRatio = static_cast<float>(width) / static_cast<float>(height);
            /*
            uint32_t cameraBuffer;

            glGenBuffers(1, &cameraBuffer);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, cameraBuffer);

            auto flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
            auto size = sizeof(RendererData::CameraInfo);

            glBufferStorage(GL_ARRAY_BUFFER, size, 0, flags);
            auto* cameraData = (RendererData::CameraInfo*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, size, flags);
            */
        }

        static GLsync gSync;

        static void LockBuffer()
        {
            if (gSync)
            {
                glDeleteSync(gSync);
            }
            gSync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
        }

        static void WaitBuffer()
        {
            if (gSync)
            {
                while (1)
                {
                    GLenum waitReturn = glClientWaitSync(gSync, GL_SYNC_FLUSH_COMMANDS_BIT, 1);
                    if (waitReturn == GL_ALREADY_SIGNALED || waitReturn == GL_CONDITION_SATISFIED)
                        return;
                }
            }
        }

        static void SetViewport(int32_t x, int32_t y, uint32_t width, uint32_t height)
        {
            glViewport(x, y, width, height);
        }
        
        static void SetClearColor(const glm::vec4& color)
        {
            glClearColor(color.r, color.g, color.b, color.a);
        }
        
        static void Draw(const Ref<Mesh>& mesh, const Ref<Shader>& shader)
        {
            shader->Bind();
            mesh->GetVertexArray()->Bind();
            auto& ib = mesh->GetIndexBuffer();
            ib->Bind();
            glDrawElements(GL_TRIANGLES, ib->GetCount(), GL_UNSIGNED_INT, nullptr);
        }
        
        static void Draw(const Mesh& mesh, const Ref<Shader>& shader)
        {
            shader->Bind();
            mesh.GetVertexArray()->Bind();
            auto& ib = mesh.GetIndexBuffer();
            ib->Bind();
            glDrawElements(GL_TRIANGLES, ib->GetCount(), GL_UNSIGNED_INT, nullptr);
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
        }

        static void Clear() { glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); }
        static void ClearDepthBuffer() { glClear(GL_DEPTH_BUFFER_BIT); }
        static ShaderLibrary& ShaderLibrary() { return s_Data.m_ShaderLibrary; }
        static float& GetGamma() { return s_Data.m_Gamma; }
        static Timer& GetTimestep() { return s_Data.timestep; }
    };

}