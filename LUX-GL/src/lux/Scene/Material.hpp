#pragma once

#include <glm/glm.hpp>

#include "lux/Types.hpp"
#include "lux/Renderer/Shader.hpp"
#include "lux/Renderer/UniformBuffer.hpp"


namespace lux {

	/*
	Every variable has to start at 0 or a 16 byte offset with a size of 16,
	except float or int. (this never seems to work very well)
	*/
	struct MaterialProperties          // size = 64
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
		MaterialProperties(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, float shininess)
			: ambient_color(ambient), diffuse_color(diffuse), specular_color(specular), specular_shininess(shininess) {}

		MaterialProperties(const glm::vec4& ambient, const glm::vec4& diffuse, const glm::vec4& specular, float shininess)
			: ambient_color(ambient), diffuse_color(diffuse), specular_color(specular), specular_shininess(shininess) {}
	};

	//float shininess = 80.0f; // Default value
    //float	metalness = 0.0f;
    //float roughness = 1.0f - glm::sqrt(shininess / 100.0f);
	//bool hasAlbedoMap

	class Material
	{
	public:
		Material(const Ref<Shader>& shader)
		{
			_props.ambient_color = { 0.348f, 0.348f, 0.348f };
			_props.diffuse_color = { 0.608f, 0.608f, 0.608f };
			_props.specular_color = { 0.5f, 0.5f, 0.5f };
			_props.specular_shininess = 32.0f;
			_ubo = UniformBuffer::Create("MaterialProperties", 4, 64, &_props);
			_ubo->AddUniform("material.ambient_color", 0, 12);
			_ubo->AddUniform("material.diffuse_color", 16, 12);
			_ubo->AddUniform("material.specular_color", 32, 12);
			_ubo->AddUniform("material.specular_shininess", 48, 4);
		}
		~Material() = default;

		glm::vec3& GetAmbientColor() { return _props.ambient_color; }
		glm::vec3& GetDiffuseColor() { return _props.diffuse_color; }
		glm::vec3& GetSpecularColor() { return _props.specular_color; }
		float& GetSpecularShininess() { return _props.specular_shininess; }
		//float& GetSpecularShininess() { return _props.specular_shininess; }

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
		bool depthTest;
	};
}