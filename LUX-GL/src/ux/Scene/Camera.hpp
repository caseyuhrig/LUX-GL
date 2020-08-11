#pragma once

#include <glm/glm.hpp>
#include "../Core.hpp"
#include "../Renderer/UniformBuffer.hpp"


namespace lux {

    //glm::vec3 camera_position = { 0.0, 0.0, 10.0 };
    //glm::vec3 camera_look_at = { 0.0, 0.0, 0.0 };   // glm::lookAt(...) ??? check this out

   

    //auto ubo_Cameras = UniformBuffer("CameraProperties", 5, 32, &cameraProperties);
    //ubo_Cameras.AddUniform("cameras[0].location", 0, 12);
    //ubo_Cameras.AddUniform("cameras[0].look_at", 16, 12);


    //uint32_t ACTIVE_CAMERA = 0;

    //ubo_Cameras.SetData(cameraProperties);



    

	// UBO struct
	typedef struct CameraProperties // size =  32
	{
		glm::vec3 position;        // 12        0
		float pad1 = 0;            //  4
		glm::vec3 look_at;         // 12       16
        //glm::mat4 projection;      // 64       64
        //glm::mat4 view;
        //glm::mat4 projection_view;
		// float pad2 = 0;         //  4
        //float zNear = 0.01f;
        //float zFar = 2000.0;
	};

    class Camera
    {
    public:      
        Camera(glm::vec3 position, glm::vec3 lootAt);
        ~Camera();
        void Publish();
        void SetAspectRatio(float aspectRatio);
        glm::mat4 GetView() const;
        glm::mat4 GetProjection() const;
        glm::vec3 GetPosition() const;
        glm::vec3 GetlookAt() const;
    private:      
        uint32_t index;
        CameraProperties _properties;
        //{ 0.0, 0.0, 10.0 }, // position
        //{ 0.0, 0.0, 0.0 }   // look_at
        //float zNear = 0.01f;
        float _z_near = 0.01f;
        float _aspect_ratio;
        Ref<UniformBuffer> _ubo;
    };
}