#pragma once

#include <GL/glew.h>

#include <iostream>
#include <string>
#include <memory>
// TODO use chrono
#include <ctime>

#include <stdarg.h>
#include <glm/glm.hpp>


//#define ASSERT(x) if (!(x)) __debugbreak();
//#define GlCall(x) GlClearError();x;ASSERT(GlLogCall(#x,__FILE__,__LINE__))


namespace ux {

	static const double PI = 3.14159265358979323846264;
	static const double PI2 = PI * 2.0;
	static const double RADIANS = PI2;
	static const double TO_RAD = PI / 180.0;
	static const double TO_DEG = 180.0 / PI;


	typedef struct SceneProperties // size = 208
	{
		glm::mat4 Model;           // 64       0
		glm::mat4 View;            // 64      64
		glm::mat4 Proj;            // 64     128
		float Gamma = 1.0f;        //  4     192
	};

	typedef struct CameraProperties // size =  32
	{
		glm::vec3 position;        // 12        0
		float pad1 = 0;            //  4
		glm::vec3 look_at;         // 12       16
		// float pad2 = 0;         //  4

		CameraProperties(glm::vec3 cp, glm::vec3 cla)
			: position(cp), look_at(cla) {}
	};

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

	/*
	Every variable has to start at 0 or a 16 byte offset with a size of 16,
	except float or int.
	*/
	typedef struct Material          // size = 64
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

		// VERY helpful! ...otherwise glm::matrix swizzling between vec3/vec4 and floats make it 
		// imposible to set via Material = {}
		Material(glm::vec3 a, glm::vec3 d, glm::vec3 s, float ss)
			: ambient_color(a), diffuse_color(d), specular_color(s), specular_shininess(ss)
		{
			std::cout << "Setting Material: Constructor 1" << std::endl;
		}

		Material(glm::vec4 a, glm::vec4 d, glm::vec4 s, float ss)
			: ambient_color(a), diffuse_color(d), specular_color(s), specular_shininess(ss)
		{
			std::cout << "Setting Material: Constructor 2" << std::endl;
		}
	};


	template<typename T>
	using Scope = std::unique_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

	
	// rotates (model) around a specified point (axis) by (angle.xyz).
	/*
	inline glm::mat4 rotate(glm::mat4 model, glm::vec3 axis, glm::vec3 angle)
	{
		//glm::mat4 rotMat = glm::rotate(glm::mat4(1.0), angle.x, glm::vec3(1.0, 0.0, 0.0));
		//rotMat = glm::rotate(rotMat, angle.y, glm::vec3(0.0, 1.0, 0.0));
		//rotMat = glm::rotate(rotMat, angle.z, glm::vec3(0.0, 0.0, 1.0));
		//rotMat = glm::translate(rotMat, axis);
		//return rotMat * model;
		return glm::translate(
			glm::rotate(
				glm::rotate(
					glm::rotate(glm::mat4(1.0), 
					    angle.x, glm::vec3(1.0, 0.0, 0.0)), 
					    angle.y, glm::vec3(0.0, 1.0, 0.0)), 
				        angle.z, glm::vec3(0.0, 0.0, 1.0)), 
			axis) * model;
	}
	*/
	
	
	
	inline std::string format(const std::string format, ...)
	{
		va_list args;	
		char buffer[50];
		va_start(args, format);
		vsprintf(buffer, format.c_str(), args);
		va_end(args);
		return std::string(buffer);
	}

	inline std::string ReadableTime()
	{
		time_t result = time(NULL);
		char str[26];
		ctime_s(str, sizeof str, &result);
		return std::string(str);
	}


	template <typename T>
	inline T random(T max)
	{
		return static_cast <T> (rand()) / static_cast <T> (RAND_MAX / max);
	}


	inline void GlClearError()
	{
		while (glGetError() != GL_NO_ERROR);
	}


	inline bool GlLogCall(const char* function, const char* file, int line)
	{
		while (unsigned int error = glGetError())
		{
			std::cout << "[OpenGL Error] (0x" << std::hex << error << ")" << std::endl;
			return false;
		}
		return true;
	}

}