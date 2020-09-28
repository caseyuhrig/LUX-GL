#pragma once

#include <glm/glm.hpp>

#include "lux/Types.hpp"
#include "lux/Renderer/UniformBuffer.hpp"
//#include "lux/Window.hpp"


namespace lux {

	struct CameraProperties
	{
		glm::vec3 position;        // 12        0
		float pad1 = 0;            //  4
		glm::vec3 look_at;         // 12       16
        float pad2 = 0;            //  4
        glm::mat4 projection;      // 64       32
        glm::mat4 view;            // 64       96
        glm::mat4 view_projection; // 64      160
        float z_near;              //  4      224
        float z_far;               //  4      228
        uint32_t viewport_width;   //  4      232
        uint32_t viewport_height;  //  4      236
        float aspect_ratio;        //  4      240
        //float z_near = 0.01f;
        //float z_far = 2000.0;

        //glm::vec3 Position;
        //glm::vec3 LookAt;
	};                             //  size = 244 (256)

    class Camera
    {
    public:
        Camera() = default;

        Camera(int width, int height, const glm::vec3& position, const glm::vec3& lookAt)
        {
            _props.viewport_width = width;
            _props.viewport_height = height;
            _props.aspect_ratio = static_cast<float>(_props.viewport_width) / static_cast<float>(_props.viewport_height);
            _props.position = position;
            _props.look_at = lookAt;
            _props.z_far = 120.0f; //2000.0f;
            _props.z_near = 0.01f;
            UpdateView();
            // 5 = binding point
            _ubo = lux::CreateRef<UniformBuffer>("CameraProperties", 5, 256, &_props);
            // TODO integrate the camera index
            _ubo->AddUniform("cameras[0].location", 0, 12);
            _ubo->AddUniform("cameras[0].look_at", 16, 12);
            _ubo->AddUniform("cameras[0].projection", 32, 64);
            _ubo->AddUniform("cameras[0].view", 96, 64);
            _ubo->AddUniform("cameras[0].view_projection", 160, 64);
            _ubo->AddUniform("cameras[0].z_near", 224, 4);
            _ubo->AddUniform("cameras[0].z_far", 228, 4);
            _ubo->AddUniform("cameras[0].viewport_width", 232, 4);
            _ubo->AddUniform("cameras[0].viewport_height", 236, 4);
            _ubo->AddUniform("cameras[0].aspect_ratio", 240, 4);
            _ubo->SetData(&_props);
            //void (*resizeFunc)(int width, int height) {
            //    this->SetViewportSize(width, height);
            //    };
            //auto func = std::bind()
            //auto func = [&](int width, int height) {
            //    this->SetViewportSize(width, height);
            //};

            //window.AddResizeListener(func);
        }
        ~Camera() = default;

      
       
        const glm::mat4& GetView() const { return _props.view; }
        //glm::mat4& GetView() { return _props.view; }
        const glm::mat4& GetProjection() const { return _props.projection; }
        const glm::mat4& GetViewProjection() const { return m_ViewProjection; } //return _props.projection * _props.view;
        const glm::vec3& GetPosition() const { return _props.position; }
        const glm::vec3& GetLookAt() const { return _props.look_at; }
        const float& GetZNear() const { return _props.z_near; }
        float& GetZNear() { return _props.z_near; }
        const float& GetZFar() const { return _props.z_far; }
        float& GetZFar() { return _props.z_far; }
        const uint32_t& GetViewportWidth() const { return _props.viewport_width; }
        //uint32_t& GetViewportWidth() { return _props.viewport_width; }
        const uint32_t& GetViewportHeight() const { return _props.viewport_width; }

        const float& GetAspectRatio() const {
            return _props.aspect_ratio;
        }

        
        void SetViewportSize(const uint32_t& width, const uint32_t& height)
        {
            _props.viewport_width = width;
            _props.viewport_height = height;
            _props.aspect_ratio = static_cast<float>(width) / static_cast<float>(height);
            UpdateView();
            PublishViewportSize();
            PublishAspectRatio();
        }
        // TODO Clean this stuff up!
        void PublishViewportSize() const 
        {
            _ubo->SetUniform1i("cameras[0].viewport_width", _props.viewport_width); 
            _ubo->SetUniform1i("cameras[0].viewport_height", _props.viewport_height);
        }

        void PublishAspectRatio() {
            UpdateView();
            _ubo->SetUniform1f("cameras[0].aspect_ratio", _props.aspect_ratio); 
        }
        void Publish() {
            UpdateView();
            _ubo->SetData(&_props);
        }
        /*
        glm::mat4 GetUpsideDownView()
        {
            return glm::lookAt(_props.position,     // Camera position in world space
                _props.look_at,      // look at origin
                glm::vec3(0, -1, 0)); // Head is up (set to 0, -1, 0 to look upside down)
        }
        */

        void UpdateView()
        {
            //_props.projection = glm::infinitePerspective(glm::radians(55.0f), _props.aspect_ratio, _props.z_near);
            _props.projection = glm::perspective(glm::radians(55.0f), _props.aspect_ratio, _props.z_near, _props.z_far);
            _props.view = glm::lookAt(_props.position,     // Camera position in world space
                _props.look_at,      // look at origin
                glm::vec3(0, 1, 0)); // Head is up (set to 0, -1, 0 to look upside down)
            m_ViewProjection = _props.projection * _props.view;
        }
    private:
        CameraProperties _props;
        Ref<UniformBuffer> _ubo;
        glm::mat4 m_ViewProjection;
    };
}