#pragma once

#include "lux/Window.hpp"


namespace lux {

    //namespace Interface {

        enum class Position {
            LEFT = 0,
            MIDDLE = 1,
            RIGHT = 2
        };
    //}

    class Interface
    {
    public:
        Interface(const std::string& title, const lux::Position& position)
        {
            switch (position)
            {
            case lux::Position::LEFT:
                // specifically set the left monitor on creation and
                // call make current on the window prior to rendering?
                m_Window = CreateRef<Window>(title, 1800, 900);
                m_Window->SetPosition(-1920 + 60, 60);
                break;
            default:
                spdlog::error("Unknown Interface Position");
            }
            m_Window->FillWorkArea();



            m_Shader = CreateRef<Shader>("res/shaders/ux.shader");

            const auto amber = glm::vec4(Colors::Amber.rgb, 0.8f);

            mesh = TopCar(glm::vec2(10.0f, 100.0f), glm::vec2(900.0f, 890.0f), amber);
            mesh.Build();

            // ORTHO
            const glm::mat4 projOrtho = glm::ortho(0.0f, static_cast<float>(m_Window->GetFramebufferWidth()), 
                0.0f, static_cast<float>(m_Window->GetFramebufferHeight()), 
                0.01f, 2000.0f); // PROJECTION (SCREEN)
            //glm::mat4 projOrtho = glm::ortho(0.f, (float)window.GetFramebufferWidth(), (float)window.GetFramebufferHeight(), 0.0f, 0.01f, 2000.0f); // PROJECTION (SCREEN)
            const glm::mat4 viewOrtho = glm::lookAt(glm::vec3(0, 0, 10.0),  // Camera position in world space (doesn't really apply when using an ortho projection matrix)
                glm::vec3(0, 0, 0),  // look at origin
                glm::vec3(0, 1, 0)); // Head is up (set to 0, -1, 0 to look upside down)       
            const glm::mat4 modelOrtho = glm::mat4(1.0f); // MODEL
            const glm::mat4 mvpOrtho = projOrtho * viewOrtho * modelOrtho;

            m_Shader->SetUniformMat4f("u_MVP", mvpOrtho);
            m_Shader->SetUniform4f("u_Color", 1.0f, 1.0f, 1.0f, 1.0f);
        }
        void Draw() const
        {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            //mesh.Draw(m_Renderer, *m_Shader);
            glDisable(GL_BLEND);
        }
        const bool& ShouldClose() const { return m_Window->ShouldClose(); }
        void Clear() const { m_Renderer.Clear(); }
        // Update was SwapBuffers
        void Update() const { m_Window->Update(); }
        void Destroy() const { m_Window->Destroy(); }
    private:
        Ref<Window> m_Window;
        Renderer m_Renderer;
        Ref<Shader> m_Shader;
        TopCar mesh;
    };
}
