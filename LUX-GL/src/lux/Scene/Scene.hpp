#pragma once

#include "glm/glm.hpp"
#include "lux/Types.hpp"
#include "lux/Scene/Camera.hpp"
#include "lux/Renderer/UniformBuffer.hpp"


namespace lux {

	typedef struct SceneProperties // size = 208
	{
		glm::mat4 Model;           // 64       0
		glm::mat4 View;            // 64      64
		glm::mat4 Proj;            // 64     128
		float Gamma = 1.0f;        //  4     192
	};


	// TODO Implement m_ActiveScene
	// save more then one scene setup in the uniform so we can jump around
	// to multiple renderings
    class Scene
    {
	public:
		Scene(SceneProperties* props, uint32_t activeScene = 0) 
			: m_Props(props), m_ActiveScene(activeScene)
		{
			m_UBO = CreateRef<UniformBuffer>("SceneProperties", 6, 208, m_Props);
			m_UBO->AddUniform("scenes[0].model", 0, 64);
			m_UBO->AddUniform("scenes[0].view", 64, 64);
			m_UBO->AddUniform("scenes[0].proj", 128, 64);
			m_UBO->AddUniform("scenes[0].gamma", 192, 4);
		}
		Scene(const Camera& camera)
			: m_ActiveScene(0)
		{
			SceneProperties* sceneProps = new SceneProperties[1];
			
			sceneProps[0] = {
				glm::mat4(1.0f),
				camera.GetView(),
				camera.GetProjection(),
				1.0f
			};
			m_Props = sceneProps;
			m_UBO = CreateRef<UniformBuffer>("SceneProperties", 6, 208, m_Props);
			m_UBO->AddUniform("scenes[0].model", 0, 64);
			m_UBO->AddUniform("scenes[0].view", 64, 64);
			m_UBO->AddUniform("scenes[0].proj", 128, 64);
			m_UBO->AddUniform("scenes[0].gamma", 192, 4);
		}
		~Scene()
		{
			delete m_Props;
		}
		void Publish() { m_UBO->SetData(m_Props); }
	private:
		Ref<UniformBuffer> m_UBO;
		SceneProperties* m_Props;
		uint32_t m_ActiveScene;
    };
}