#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <imgui/imgui.h>

#include "lux/Renderer/Renderer.hpp"
#include "lux/Scene/Layer.hpp"
#include "lux/Window.hpp"
#include "lux/Primitive/BezierCurve.hpp"


namespace lux {

    class LcarsLayer : public Layer
    {
    public:
        LcarsLayer(const uint32_t width, const uint32_t height, glm::vec3& rotate) 
            : Layer("LCARS"), m_Width(width), m_Height(height), m_Rotate(rotate)
        {
            m_Shader = Renderer::ShaderLibrary().Get("UX");
            m_ShaderBasic = Renderer::ShaderLibrary().Get("Basic");

            auto amber = glm::vec4(Colors::Amber.rgb, 0.8f);

            topCar = TopCar(glm::vec2(10.0f, 10.0f), glm::vec2(1100.0f, 1100.0f), amber);
            //topCar.Build();

            // ORTHO
            //glm::mat4 projOrtho = glm::ortho(0.f, static_cast<float>(m_Width), 0.0f, static_cast<float>(m_Height), 0.01f, 2000.0f); // PROJECTION (SCREEN)
            //glm::mat4 viewOrtho = glm::lookAt(glm::vec3(0, 0, 10.0),  // Camera position in world space (doesn't really apply when using an ortho projection matrix)
            //    glm::vec3(0, 0, 0),  // look at origin
            //    glm::vec3(0, 1, 0)); // Head is up (set to 0, -1, 0 to look upside down)       
            //glm::mat4 modelOrtho = glm::mat4(1.0f); // MODEL
            //glm::mat4 mvpOrtho = projOrtho * viewOrtho * modelOrtho;

            //m_Shader->SetUniformMat4f("u_MVP", mvpOrtho);
            m_Shader->SetUniform4f("u_Color", 1.0f, 1.0f, 1.0f, 1.0f);

            //m_ShaderBasic->SetUniformMat4f("u_MVP", mvpOrtho);
            m_ShaderBasic->SetUniform4f("u_Color", 1.0f, 1.0f, 1.0f, 1.0f);

            textList = TextList(110, 30, width, height);
            textList.AddFont(0, "res/fonts/Inconsolata/static/InconsolataCondensed-Medium.ttf");
            textList.AddFont(1, "res/fonts/Inconsolata/static/InconsolataCondensed-Light.ttf");
            textList.AddText(0, 0, 24, 24, "LUX/GL v0.12a");
            textList.AddText(1, 1, 18, 18, "LCARS USER EXPERIENCE GRAPHICS LIBRARY");
            textList.AddText(1, 2, 18, 18, "MXR: 0.000000");
            textList.AddText(1, 3, 18, 18, "MYR: 0.000000");
            textList.AddText(1, 4, 18, 18, "MZR: 0.000000");
            textList.AddText(1, 5, 18, 18, "MS/FR: 0.000000");
            textList.AddText(1, 6, 18, 18, "FPS: 0.000000");
            textList.AddText(1, 7, 18, 18, "[DATE/TIME]");

            Resize(width, height);
        }

        void Resize(const uint32_t width, const uint32_t height)
        {
            m_Width = width;
            m_Height = height;
            textList.Resize(width, height);
            // ORTHO
            //auto proj = glm::ortho(0.f, static_cast<float>(m_Width), 0.0f, static_cast<float>(m_Height), 0.01f, 2000.0f); // PROJECTION (SCREEN)
            auto proj = glm::ortho(0.f, static_cast<float>(m_Width), static_cast<float>(m_Height), 0.0f, 0.01f, 2000.0f);
 
            auto view = glm::lookAt(glm::vec3(0, 0, 10.0),  // Camera position in world space (doesn't really apply when using an ortho projection matrix)
                glm::vec3(0, 0, 0),  // look at origin
                glm::vec3(0, 1, 0)); // Head is up (set to 0, -1, 0 to look upside down)       
            //glm::mat4 model = glm::mat4(1.0f); // MODEL
            glm::mat4 mvp = proj * view; // *model;
            m_Shader->SetUniformMat4f("u_MVP", mvp);
            m_ShaderBasic->SetUniformMat4f("u_MVP", mvp);
        }

        void SetLights(const Lights& lights)
        {
            m_Lights = lights;
        }
        
        void SetRotate(glm::vec3& rotate)
        {
            m_Rotate = rotate;
        }
        
        void OnUpdate() override
        {
            //glDisable(GL_BLEND);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            //scoopedCorner.Draw(renderer, shaderUX);
            //roundedCorner.Draw(renderer, shaderUX);
            //roundedCorner2.Draw(renderer, shaderUX);
            //horzBar.Draw(renderer, shaderUX);
            //topCar.Draw(m_Shader);
            Renderer::Draw(topCar, m_Shader);
            glDisable(GL_BLEND);

            //curve.Draw(m_ShaderBasic);

            //if (imguiLayer.showText)
            //{
                //glm::vec4 color = { 0.2, 0.2, 0.2, 1.0 };
                // TODO text can be made faster here!
                textList.SetText(2, fmt::sprintf("MODEL: % 12.6f % 12.6f % 12.6f", m_Rotate[0], m_Rotate[1], m_Rotate[2]));
                textList.SetText(3, fmt::sprintf("LIGHT: % 12.6f % 12.6f % 12.6f", m_Lights.GetPosition(0).x, m_Lights.GetPosition(0).y, m_Lights.GetPosition(0).z));
                //textList.SetText(4, fmt::sprintf("COLOR: %6.4f %1.4f %1.4f %1.4f", color[0], color[1], color[2], color[3]));
                float msFrame = 1000.0f / ImGui::GetIO().Framerate;
                float frameRate = ImGui::GetIO().Framerate;
                textList.SetText(5, fmt::sprintf("MS/FR: %8.6f", msFrame));
                textList.SetText(6, fmt::sprintf("  FPS: %8.3f", frameRate));
                textList.SetText(7, lux::StringUtils::ToUppercase(lux::Time::Readable()));

                textList.Draw();
        }
    private:
        uint32_t m_Width;
        uint32_t m_Height;
        Ref<Shader> m_Shader;
        Ref<Shader> m_ShaderBasic;

        TextList textList;
        Lights m_Lights;
        glm::vec3& m_Rotate;
        lux::TopCar topCar;

        Primitive::BezierCurve curve;
    };
}