#pragma once

#include <glad/glad.h>

#include <iostream>
#include <string>
#include <algorithm>
#include <memory>
#include <ctime>

#include <stdarg.h>
#include <glm/glm.hpp>


//#define ASSERT(x) if (!(x)) __debugbreak();



namespace lux {

	static const double PI = 3.14159265358979323846264;
	static const double PI2 = PI * 2.0;
	static const double RADIANS = PI2;
	static const double TO_RAD = PI / 180.0;
	static const double TO_DEG = 180.0 / PI;

	static const float PIf = 3.14159265358979323846264f;
	static const float PI2f = PIf * 2.0f;
	static const float RADIANSf = PI2f;
	static const float TO_RADf = PIf / 180.0f;
	static const float TO_DEGf = 180.0f / PIf;

	typedef struct SceneProperties // size = 208
	{
		glm::mat4 Model;           // 64       0
		glm::mat4 View;            // 64      64
		glm::mat4 Proj;            // 64     128
		float Gamma = 1.0f;        //  4     192
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

	template <typename T>
	T random(T max)
	{
		return static_cast <T> (rand()) / static_cast <T> (RAND_MAX / max);
	}

}