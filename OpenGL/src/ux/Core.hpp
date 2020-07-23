#pragma once

#include <GL/glew.h>

#include <iostream>
#include <string>
#include <memory>
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

	/*
	// rotates (model) around a specified point (axis) by (angle.xyz).
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