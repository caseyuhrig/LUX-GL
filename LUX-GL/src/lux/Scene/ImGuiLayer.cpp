#include "ImGuiLayer.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
//#define IMAPP_IMPL
//#include "imgui/ImApp.h"
//#include "imgui/ImGuizmo.h"

#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"


namespace lux {

    ImGuiLayer::ImGuiLayer(Window* window) : Layer(window->GetWidth(), window->GetHeight())
    {
        _window = window;
        SetDebugLabel("ImGui Layer");
    }

    void ImGuiLayer::Resize(const uint32_t& width, const uint32_t& height)
    {

    }

    void ImGuiLayer::Begin()
    {
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        const char* glsl_version = "#version 450";
        ImGui::CreateContext();
        //ImGuiIO& io = ImGui::GetIO(); (void)io;
        // Setup Platform/Renderer bindings
        ImGui_ImplGlfw_InitForOpenGL(_window->GetNativeWindow(), true);
        ImGui_ImplOpenGL3_Init(glsl_version);
        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsClassic();
    }

    void ImGuiLayer::SetCubemapSwapFunction(void (*callback_function)(void))
    {
        callback_function();
    }

    void ImGuiLayer::End()
    {

    }

    void ImGuiLayer::Draw() const
    {
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        {
            ImGuiColorEditFlags misc_flags = ImGuiColorEditFlags_Float;

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
            
            ImGui::SliderFloat3("Camera Position", (float*)&_camera->GetPosition(), -100.0f, 100.0f);
            if (ImGui::IsItemActive()) { _camera->UpdateView(); }
            ImGui::SliderFloat3("Camera Look At", (float*)&_camera->GetLookAt(), -100.0f, 100.0f);
            if (ImGui::IsItemActive()) { _camera->UpdateView(); }
            ImGui::SliderFloat("Z Near", &_camera->GetZNear(), 0.01f, 2000.0f);
            ImGui::SliderFloat("Z Far", &_camera->GetZFar(), 0.01f, 2000.0f);
            
            //glm::vec3 lightInc = glm::vec3(0.045, 0.0, 0.0);
            //glm::vec3 lpos = _lights->GetPosition(0);
            
            ImGui::Separator();
            ImGui::SliderFloat3("Light Position", (float*)m_LightPos, -100.0f, 100.0f);
            
            ImGui::Separator();
            ImGui::SliderInt("Shadow Samples", m_ShadowSamples, 1.0f, 100.0f);
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
            if (ImGui::Button("Exit"))
            {
                std::cout << "EXIT" << std::endl;
                //break;
                //exit(0);
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

            ImGui::ShowDemoWindow();
        }

        ImGui::Render();
        ImGui::EndFrame();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
}