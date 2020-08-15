#pragma once

#include <glm/glm.hpp>

#include "../Core.hpp"
#include "../Renderer/UniformBuffer.hpp"


namespace lux {

	/*
	Every variable has to start at 0 or a 16 byte offset with a size of 16,
	except float or int. (this never seems to work very well)
	*/
	typedef struct MaterialProperties          // size = 64
	{
		glm::vec3 ambient_color;     // 12      0
		float pad1 = 0;              //  4
		glm::vec3 diffuse_color;     // 12     16
		float pad2 = 0;              //  4
		glm::vec3 specular_color;    // 12     32
		float pad3 = 0;              //  4
		float specular_shininess;    //  4     48
		float pad4 = 0;              //  4
		float pad5 = 0;              //  4
		float pad6 = 0;              //  4
		//float ambient_strength = 0.1;
		//float specular_strength = 0.5;

		MaterialProperties()
			: ambient_color(0), diffuse_color(0), specular_color(0), specular_shininess(0) {}

		// VERY helpful! ...otherwise glm::matrix swizzling between vec3/vec4 and floats make it 
		// imposible to set via Material = {}
		MaterialProperties(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float shininess)
			: ambient_color(ambient), diffuse_color(diffuse), specular_color(specular), specular_shininess(shininess) {}

		MaterialProperties(glm::vec4 ambient, glm::vec4 diffuse, glm::vec4 specular, float shininess)
			: ambient_color(ambient), diffuse_color(diffuse), specular_color(specular), specular_shininess(shininess) {}
	};

	class Material
	{
	public:
		Material()
		{
			_props.ambient_color = { 0.348f, 0.348f, 0.348f };
			_props.diffuse_color = { 0.608f, 0.608f, 0.608f };
			_props.specular_color = { 0.5f, 0.5f, 0.5f };
			_props.specular_shininess = 32.0f;
			_ubo = CreateRef<UniformBuffer>("MaterialProperties", 4, 64, &_props);
			_ubo->AddUniform("material.ambient_color", 0, 12);
			_ubo->AddUniform("material.diffuse_color", 16, 12);
			_ubo->AddUniform("material.specular_color", 32, 12);
			_ubo->AddUniform("material.specular_shininess", 48, 4);
		}
		~Material() = default;

		const glm::vec3& GetAmbientColor() const { return _props.ambient_color; }
		const glm::vec3& GetDiffuseColor() const { return _props.diffuse_color; }
		const glm::vec3& GetSpecularColor() const { return _props.specular_color; }
		const float& GetSpecularShininess() const { return _props.specular_shininess; }
		float& GetSpecularShininess() { return _props.specular_shininess; }

		void SetAmbientColor(const glm::vec4& ambientColor) { _props.ambient_color = ambientColor;	PublishAmbientColor();}
		void SetDiffuseColor(const glm::vec4& diffuseColor) { _props.diffuse_color = diffuseColor;	PublishDiffuseColor(); }
		void SetSpecularColor(const glm::vec4& specularColor) { _props.specular_color = specularColor;	PublishSpecularColor(); }
		void SetSpecularShininess(const float& specularShininess) { _props.specular_shininess = specularShininess;	PublishSpecularShininess(); }
		
		void PublishAmbientColor() const { _ubo->SetUniformVec3("material.ambient_color", _props.ambient_color); }
		void PublishDiffuseColor() const { _ubo->SetUniformVec3("material.diffuse_color", _props.diffuse_color); }
		void PublishSpecularColor() const { _ubo->SetUniformVec3("material.specular_color", _props.specular_color); }
		void PublishSpecularShininess() const { _ubo->SetUniform1f("material.specular_shininess", _props.specular_shininess); }
	private:
		MaterialProperties _props;
		Ref<UniformBuffer> _ubo;
	};
}