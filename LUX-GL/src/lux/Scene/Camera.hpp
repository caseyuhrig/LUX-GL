#pragma once

#include <glm/glm.hpp>

#include "lux/Types.hpp"
#include "lux/Renderer/UniformBuffer.hpp"
#include "lux/Log.hpp"


namespace lux {

	struct CameraSpecification
	{
		glm::vec3 Position;        // 12        0
		float pad1 = 0;            //  4
		glm::vec3 LookAt;          // 12       16
        float pad2 = 0;            //  4
        glm::mat4 Projection;      // 64       32
        glm::mat4 View;            // 64       96
        glm::mat4 ViewProjection;  // 64      160
        float zNear;               //  4      224
        float zFar;                //  4      228
        uint32_t ViewportWidth;    //  4      232
        uint32_t ViewportHeight;   //  4      236
        float AspectRatio;         //  4      240
        float Angle;               //  4      244
        float pad3 = 0;            //  4      248
        float pad4 = 0;            //  4      252
        glm::vec3 Up;              // 12      256
        float pad5 = 0;            //  4      268
	};                             //  size = 272 (16x7)

    class Camera : public Creatable<Camera>
    {
    public:
        /*
        static auto Create(
            const uint32_t width, 
            const uint32_t height, 
            const glm::vec3& position, 
            const glm::vec3& lookAt,
            const uint32_t bindingPoint = 5)
        { 
            return CreateRef<Camera>(width, height, position, lookAt); 
        }
        */
        Camera() = default;

        Camera(
            const uint32_t width, 
            const uint32_t height, 
            const glm::vec3& position, 
            const glm::vec3& lookAt, 
            const uint32_t bindingPoint = 5)
        {
            m_Spec.ViewportWidth = width;
            m_Spec.ViewportHeight = height;
            m_Spec.AspectRatio = static_cast<float>(width) / static_cast<float>(height);
            m_Spec.Position = position;
            m_Spec.LookAt = lookAt;
            m_Spec.zFar = 2000.0f; // [2000.0f]
            m_Spec.zNear = 0.01f;  // [0.01f]
            m_Spec.Angle = 55.0f;  // [55.0f]
            m_Spec.Up = glm::vec3(0, 1, 0); // [(0, 1, 0)]
            CalcViewProjection();

            auto size = sizeof(CameraSpecification);
            spdlog::debug("Camera Spec Size {}", size);

            m_UniformBuffer = UniformBuffer::Scope("CameraProperties", bindingPoint, size, &m_Spec);
            // TODO integrate the camera index
            m_UniformBuffer->AddUniform("cameras[0].location", 0, 12);
            m_UniformBuffer->AddUniform("cameras[0].look_at", 16, 12);
            m_UniformBuffer->AddUniform("cameras[0].projection", 32, 64);
            m_UniformBuffer->AddUniform("cameras[0].view", 96, 64);
            m_UniformBuffer->AddUniform("cameras[0].view_projection", 160, 64);
            m_UniformBuffer->AddUniform("cameras[0].z_near", 224, 4);
            m_UniformBuffer->AddUniform("cameras[0].z_far", 228, 4);
            m_UniformBuffer->AddUniform("cameras[0].viewport_width", 232, 4);
            m_UniformBuffer->AddUniform("cameras[0].viewport_height", 236, 4);
            m_UniformBuffer->AddUniform("cameras[0].aspect_ratio", 240, 4);
            m_UniformBuffer->AddUniform("cameras[0].angle", 244, 4);
            m_UniformBuffer->AddUniform("cameras[0].up", 256, 12);
            m_UniformBuffer->SetData(&m_Spec);
        }
        ~Camera() = default;
      
        const glm::mat4& GetView() const { return m_Spec.View; }
        glm::mat4& GetView() { return m_Spec.View; }
        const glm::mat4& GetProjection() const { return m_Spec.Projection; }
        const glm::mat4& GetViewProjection() const { return m_Spec.ViewProjection; }
        const glm::vec3& GetPosition() const { return m_Spec.Position; }
        const glm::vec3& GetLookAt() const { return m_Spec.LookAt; }
        const float& GetZNear() const { return m_Spec.zNear; }
        float& GetZNear() { return m_Spec.zNear; }
        const float& GetZFar() const { return m_Spec.zFar; }
        float& GetZFar() { return m_Spec.zFar; }
        const uint32_t& GetViewportWidth() const { return m_Spec.ViewportWidth; }
        const uint32_t& GetViewportHeight() const { return m_Spec.ViewportWidth; }
        float& GetAngle() { return m_Spec.Angle; }
        const float& GetAngle() const { return m_Spec.Angle; }
        const float& GetAspectRatio() const { return m_Spec.AspectRatio; }
        
        void SetViewportSize(const uint32_t width, const uint32_t height)
        {
            m_Spec.ViewportWidth = width;
            m_Spec.ViewportHeight = height;
            m_Spec.AspectRatio = static_cast<float>(width) / static_cast<float>(height);
            Publish();
        }
        void Publish() 
        {
            CalcViewProjection();
            m_UniformBuffer->SetData(&m_Spec);
        }
    protected:
        void CalcViewProjection()
        {
            m_Spec.Projection = glm::perspective(glm::radians(m_Spec.Angle), m_Spec.AspectRatio, m_Spec.zNear, m_Spec.zFar);
            m_Spec.View = glm::lookAt(m_Spec.Position, m_Spec.LookAt, glm::vec3(0, 1, 0));
            m_Spec.ViewProjection = m_Spec.Projection * m_Spec.View;
        }
    private:
        CameraSpecification m_Spec;
        Scope<UniformBuffer> m_UniformBuffer;
    };
}