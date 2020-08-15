#pragma once

#include <glm/glm.hpp>


namespace lux {

	typedef struct Light           // size =  64
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

		Light(glm::vec3 p, glm::vec3 a, glm::vec3 d, glm::vec3 s)
			: position(p, 1.0f), ambient_color(a), diffuse_color(d), specular_color(s)
		{}
	};

}