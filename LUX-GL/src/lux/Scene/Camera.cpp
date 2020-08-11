#include "Camera.hpp"

#include <glm/gtc/matrix_transform.hpp>


namespace lux {

    Camera::Camera(glm::vec3 position, glm::vec3 lookAt) 
    {
        _properties.position = position;
        _properties.look_at = lookAt;
        _ubo = lux::CreateRef<UniformBuffer>("CameraProperties", 5, 32, &_properties);
        // TODO integrate the camera index
        _ubo->AddUniform("cameras[0].location", 0, 12);
        _ubo->AddUniform("cameras[0].look_at", 16, 12);
        _ubo->SetData(&_properties);
    }

    Camera::~Camera() 
    {
        // TODO Delete the Ref?  Still havent learned exactly what these are vs. normal new/delete
        //      need to back and do som reading/watch tutorial.
    }

    glm::vec3 Camera::GetPosition() const
    {
        return _properties.position;
    }

    glm::vec3 Camera::GetlookAt() const
    {
        return _properties.look_at;
    }

    glm::mat4 Camera::GetProjection() const
    {
        //glm::mat4 proj = glm::perspective(glm::radians(55.0f), window.GetAspectRatio(), zNear, zFar); // PROJECTION (SCREEN)
        glm::mat4 projection = glm::infinitePerspective(glm::radians(55.0f), _aspect_ratio, _z_near); // looks the same
        // also look at glm::frustum(...) matrix, glm::perspectiveFov(...)
        return projection;
    }

    glm::mat4 Camera::GetView() const
    {
        glm::mat4 view = glm::lookAt(_properties.position,     // Camera position in world space
            _properties.look_at,      // look at origin
            glm::vec3(0, 1, 0)); // Head is up (set to 0, -1, 0 to look upside down)

        //glm::mat4 viewTranslate = glm::translate(viewBase, glm::vec3(0.0, 0.0, translateZ));
        //glm::mat4 viewRotateX = glm::rotate(viewTranslate, rotateXY.x, glm::vec3(1.0, 0.0, 0.0));
        //glm::mat4 view = glm::rotate(viewRotateX, rotateXY.y, glm::vec3(0.0, 1.0, 0.0)); // CAMERA
        return view;
    }

    void Camera::SetAspectRatio(float aspectRatio)
    {
        _aspect_ratio = aspectRatio;
    }

    void Camera::Publish()
    {
        _ubo->SetData(&_properties);
    }
}