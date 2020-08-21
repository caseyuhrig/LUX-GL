#pragma once

#include <glm/glm.hpp>


namespace lux {

	typedef struct LightProperties           // size =  64
	{
		glm::vec4 position;        // 16       0
		glm::vec3 ambient_color;   // 12      16
		float pad2 = 0;            //  4
		glm::vec3 diffuse_color;   // 12      32
		float pad3 = 0;            //  4
		glm::vec3 specular_color;  // 12      48
		float pad4 = 0;            //  4
		//float l_inear;
		//float quadratic;
		//float radius;

		LightProperties(glm::vec3 p, glm::vec3 a, glm::vec3 d, glm::vec3 s)
			: position(p, 1.0f), ambient_color(a), diffuse_color(d), specular_color(s)
		{}
	};

	class Lights
	{
	public:
		Lights(LightProperties* props) : m_Props(props)
		{
			m_UBO = CreateRef<UniformBuffer>("LightProperties", 3, 128, m_Props);
			m_UBO->AddUniform("lights[0].position", 0, 16);
			m_UBO->AddUniform("lights[0].ambient_color", 16, 12);
			m_UBO->AddUniform("lights[0].diffuse_color", 32, 12);
			m_UBO->AddUniform("lights[0].specular_color", 48, 12);

			m_UBO->AddUniform("lights[1].position", 64, 16);
			m_UBO->AddUniform("lights[1].ambient_color", 80, 12);
			m_UBO->AddUniform("lights[1].diffuse_color", 96, 12);
			m_UBO->AddUniform("lights[1].specular_color", 112, 12);
		}
		void Publish() { m_UBO->SetData(m_Props); }

		// TODO Fixed index
		void SetPosition(uint32_t index, glm::vec3 position)
		{
			switch (index) {
				case 0: m_UBO->SetUniformVec4("lights[0].position", glm::vec4(position, 1.0)); break;
				case 1: m_UBO->SetUniformVec4("lights[1].position", glm::vec4(position, 1.0)); break;
				default: UX_LOG_ERROR("Light index %d not found.", index);
			}
		}

		glm::vec3 GetPosition(uint32_t index) { return glm::vec3(m_Props[index].position); }
	private:
		Ref<UniformBuffer> m_UBO;
		LightProperties* m_Props;
	};
}