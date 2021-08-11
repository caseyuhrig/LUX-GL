#pragma once

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "Layer.hpp"
#include "lux/Scene/Material.hpp"
#include "lux/Window.hpp"
#include "lux/Scene/Camera.hpp"
#include "lux/Scene/Light.hpp"
#include "lux/Renderer/Shader.hpp"


namespace lux {

    class ImGuiLayer : public Layer
    {
    public:
        ImGuiLayer(Window* window) : Layer("ImGui Layer")
        {
            m_Window = window;
        }
        ~ImGuiLayer() = default;
        
        

        void OnAttach() override
        {
            // Setup Dear ImGui context
            IMGUI_CHECKVERSION();
            std::string glsl_version = "#version 450";
            ImGui::CreateContext();
            //ImGuiIO& io = ImGui::GetIO(); (void)io;
            // Setup Platform/Renderer bindings
            ImGui_ImplGlfw_InitForOpenGL(m_Window->GetGlfwWindow(), true);
            ImGui_ImplOpenGL3_Init(glsl_version.c_str());
            // Setup Dear ImGui style
            ImGui::StyleColorsDark();
            //ImGui::StyleColorsClassic();
        }

        void OnDetach() override
        {
            ImGui_ImplOpenGL3_Shutdown();
            ImGui_ImplGlfw_Shutdown();
            ImGui::DestroyContext();
        }

        void BeginFrame()
        {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            //ImGuizmo::BeginFrame();
        }

        void EndFrame()
        {
            //ImGuiIO& io = ImGui::GetIO();
            //Application& app = Application::Get();
            //io.DisplaySize = ImVec2(app.GetWindow().GetWidth(), app.GetWindow().GetHeight());
          
            // Rendering
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            
            //if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
            //{
            //    GLFWwindow* backup_current_context = glfwGetCurrentContext();
            //    ImGui::UpdatePlatformWindows();
            //    ImGui::RenderPlatformWindowsDefault();
            //    glfwMakeContextCurrent(backup_current_context);
            //}
        }

        void OnImGuiRender() override
        {
            ImGui::Begin("UX/SIM/GL");
            /*
            //ImGui::SliderFloat4("ABCD", abcd, -3.0f, 3.0f, "%.6f");
            //ImGui::SliderFloat4("EFGH", efgh, -3.0f, 3.0f, "%.6f");
            //ImGui::ColorEdit3("Color", (float*)&color, misc_flags);
            */
            ImGui::SliderFloat("M1 Scale", _scale1, 1.0f, 50.0f);
            ImGui::SliderFloat3("M1 Translate", (float*)_translate1, -20.0f, 20.0f);
            ImGui::SliderFloat3("M1 Rotate", (float*)_rotate1, 0.0f, lux::PI2f);
            ImGui::Separator();
            ImGui::SliderFloat("M2 Scale", _scale2, 1.0f, 50.0f);
            ImGui::SliderFloat3("M2 Translate", (float*)_translate2, -20.0f, 20.0f);
            ImGui::SliderFloat3("M2 Rotate", (float*)_rotate2, 0.0f, lux::PI2f);
            ImGui::Separator();

            ImGui::SliderFloat3("Cube Scale", (float*)m_ScaleCube, 1.0f, 10.0f);
            ImGui::SliderFloat3("Cube Translate", (float*)m_TranslateCube, -10.0f, 10.0f);
            ImGui::SliderFloat3("Cube Rotate", (float*)m_RotateCube, 0.0f, lux::PI2f);
            ImGui::Separator();

            // MATERIAL
            ImGui::ColorEdit3("Ambient Color", (float*)&_material->GetAmbientColor(), misc_flags);
            if (ImGui::IsItemActive()) _material->PublishAmbientColor();
            ImGui::ColorEdit3("Diffuse Color", (float*)&_material->GetDiffuseColor(), misc_flags);
            if (ImGui::IsItemActive()) _material->PublishDiffuseColor();
            ImGui::ColorEdit3("Specular Color", (float*)&_material->GetSpecularColor(), misc_flags);
            if (ImGui::IsItemActive()) _material->PublishSpecularColor();
            ImGui::SliderFloat("Specular Shininess", &_material->GetSpecularShininess(), 0.0f, 100.0f);
            if (ImGui::IsItemActive()) _material->PublishSpecularShininess();
            ImGui::Separator();

            ImGui::SliderFloat3("Camera Position", (float*)&m_Camera->GetPosition(), -100.0f, 100.0f);
            if (ImGui::IsItemActive()) { m_Camera->Publish(); }
            ImGui::SliderFloat3("Camera Look At", (float*)&m_Camera->GetLookAt(), -100.0f, 100.0f);
            if (ImGui::IsItemActive()) { m_Camera->Publish(); }
            ImGui::SliderFloat("Angle", &m_Camera->GetAngle(), 1.0f, 180.0f);
            if (ImGui::IsItemActive()) { m_Camera->Publish(); }
            ImGui::SliderFloat("Z Near", &m_Camera->GetZNear(), 0.01f, 2000.0f);
            if (ImGui::IsItemActive()) { m_Camera->Publish(); }
            ImGui::SliderFloat("Z Far", &m_Camera->GetZFar(), 0.01f, 2000.0f);
            if (ImGui::IsItemActive()) { m_Camera->Publish(); }

            //glm::vec3 lightInc = glm::vec3(0.045, 0.0, 0.0);
            //glm::vec3 lpos = _lights->GetPosition(0);

            ImGui::Separator();
            ImGui::SliderFloat3("Light Position", (float*)m_LightPos, -100.0f, 100.0f);

            ImGui::Separator();
            ImGui::SliderInt("Shadow Samples", m_ShadowSamples, 1, 100);
            ImGui::SliderFloat("Shadow Bias", m_ShadowBias, 0.0f, 1.0f);
            /*
            ImGui::SliderFloat3("Light Position", (float*)&_lights->GetPosition(0), -100.0f, 100.0f);
            if (ImGui::IsItemActive()) {
                _lights_UBO->SetUniformVec4("lights[0].position", glm::vec4(lpos, 1.0f));
                //ubo_Awesome.SetUniform("ux_light_pos", glm::vec4(lightPos, 1.0f));
                _shader_base->Bind();
                _shader_base->SetUniformMat4f("u_model", glm::translate(glm::mat4(1.0f), lpos));
            }
            ImGui::ColorEdit3("Light Ambient", (float*)&_light->ambient_color, misc_flags);
            if (ImGui::IsItemActive()) _lights_UBO->SetUniformVec3("lights[0].ambient_color", _light->ambient_color);

            ImGui::ColorEdit3("Light Diffuse", (float*)&_light->diffuse_color, misc_flags);
            if (ImGui::IsItemActive()) _lights_UBO->SetUniformVec3("lights[0].diffuse_color", _light->diffuse_color);
            ImGui::ColorEdit3("Light Specular", (float*)&_light->specular_color, misc_flags);
            if (ImGui::IsItemActive()) _lights_UBO->SetUniformVec3("lights[0].specular_color", _light->specular_color);
            ImGui::SliderFloat3("Light Increment", (float*)&lightInc, -2.0f, 2.0f);
            */
            ImGui::Separator();
            ImGui::Checkbox("Skybox", (bool*)&useSkybox); ImGui::SameLine();
            ImGui::Checkbox("Cubemap", (bool*)&useCubemap);
            ImGui::Checkbox("Shadows", (bool*)&useShadows);
            ImGui::Separator();
            ImGui::Checkbox("Animate", (bool*)&animate); ImGui::SameLine();
            ImGui::Checkbox("Show Text", (bool*)&showText);
            //ImGui::SliderFloat("Gamma", &renderer.Gamma(), 0.0f, 5.0f);
            //if (ImGui::IsItemActive()) ubo_SceneProperties.SetUniform1f("scenes[0].gamma", renderer.Gamma());
            /*
            ImGui::ColorEdit3("s_color1", (float*)&color, misc_flags);
            //if (ImGui::IsItemActive()) ubo_Super.SetUniform("s_color1", color);
            ImGui::Separator();
            ImGui::Checkbox("Animate", &animate);
            ImGui::SameLine();
            ImGui::Checkbox("Show Text", &show_text);
            ImGui::SliderFloat("Rotator Increment", &rotator_increment, -2, 2);
            ImGui::SliderFloat("Gamma", &renderer.Gamma(), 0.0f, 5.0f);
            if (ImGui::IsItemActive()) ubo_SceneProperties.SetUniform1f("scenes[0].gamma", renderer.Gamma());
            /*
            if (ImGui::Button("Play Sound"))
            {
                auto soundPos = irrklang::vec3df(lightPos.x, lightPos.y, lightPos.z);
                engine->play3D(soundFilename.c_str(), soundPos);
            }
            */
            if (ImGui::Button("Resize"))
            {
                m_Window->SuperSize();
            }
            if (ImGui::Button("Exit"))
            {
                std::cout << "EXIT" << std::endl;
                //break;
                //exit(0);
                m_Window->Close();
            }

            //ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            /*
            int m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
            float m_SnapValue = 0.5f;
            float rw = (float)ImGui::GetWindowWidth();
            float rh = (float)ImGui::GetWindowHeight();
            ImGuizmo::SetOrthographic(false);
            ImGuizmo::SetDrawlist();
            ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, rw, rh);
            bool snap = false; //Input::IsKeyPressed(HZ_KEY_LEFT_CONTROL);

            ImGuizmo::DrawGrid(&view[0][0], &proj[0][0], &model[0][0], 100.f);
            ImGuizmo::DrawCubes(&view[0][0], &proj[0][0], &model[0][0], 1);
            ImGuizmo::Manipulate(glm::value_ptr(view * model),
                glm::value_ptr(proj),
                (ImGuizmo::OPERATION)m_GizmoType,
                ImGuizmo::LOCAL,
                glm::value_ptr(model),
                nullptr,
                snap ? &m_SnapValue : nullptr);
            */
            /*
            ImGuizmo::Manipulate(glm::value_ptr(m_ActiveScene->GetCamera().GetViewMatrix()* m_MeshEntity->Transform()),
                glm::value_ptr(m_ActiveScene->GetCamera().GetProjectionMatrix()),
                (ImGuizmo::OPERATION)m_GizmoType,
                ImGuizmo::LOCAL,
                glm::value_ptr(*m_CurrentlySelectedTransform),
                nullptr,
                snap ? &m_SnapValue : nullptr);
                */
                //ImGuizmo::DrawGrid(cameraView, cameraProjection, identityMatrix, 100.f);
                //ImGuizmo::DrawCubes(cameraView, cameraProjection, &objectMatrix[0][0], gizmoCount);
            ImGui::End();
        }

        void SetCubeTransforms(glm::vec3* scaleCube, glm::vec3* translateCube, glm::vec3* rotateCube)
        {
            m_ScaleCube = scaleCube;
            m_TranslateCube = translateCube;
            m_RotateCube = rotateCube;
        }

        void Set1(float* scale, glm::vec3* translate, glm::vec3* rotate) {
            _scale1 = scale;
            _translate1 = translate;
            _rotate1 = rotate;
        }
        void Set2(float* scale, glm::vec3* translate, glm::vec3* rotate) {
            _scale2 = scale;
            _translate2 = translate;
            _rotate2 = rotate;
        }

        void SetShadowSamples(int* shadowSamples) { m_ShadowSamples = shadowSamples; }
        void SetShadowBias(float* shadowBias) { m_ShadowBias = shadowBias; }

        void SetCamera(const Ref<Camera>& camera) { m_Camera = camera; }
        void SetMaterial(Material* material) { _material = material; }
        void SetLights(Lights* lights) { _lights = lights; }
        void SetLightPos(glm::vec3* lightPos) { m_LightPos = lightPos; }
        void SetShaderBase(const Ref<Shader>& shader) { _shader_base = shader; }

        //void SetCubemapSwapFunction(void (*callback_function)(void));

        bool useShadows = true;
        bool useSkybox = false;
        bool useCubemap = false;
        bool animate = true;
        bool showText = true;
    private:
        //Ref<Window> m_Window;
        Window* m_Window;
        Ref<Camera> m_Camera;
        Lights* _lights;
        Material* _material;
        UniformBuffer* _lights_UBO;
        Ref<Shader> _shader_base;

        glm::vec3* m_LightPos;

        glm::vec3* m_ScaleCube;
        glm::vec3* m_TranslateCube;
        glm::vec3* m_RotateCube;

        float* _scale1;
        glm::vec3* _translate1;
        glm::vec3* _rotate1;

        float* _scale2;
        glm::vec3* _translate2;
        glm::vec3* _rotate2;

        float* m_ShadowBias;
        int* m_ShadowSamples;

        ImGuiColorEditFlags misc_flags = ImGuiColorEditFlags_Float;
    };
}