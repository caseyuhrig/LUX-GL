﻿#pragma once

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

	static const double PI = 3.14159265358979323846264; // std::acos(-1.0);
	static const double PI2 = PI * 2.0;
	static const double RADIANS = PI2;
	static const double TO_RAD = PI / 180.0;
	static const double TO_DEG = 180.0 / PI;

	static const float PIf = 3.14159265358979323846264f; // std::acosf(-1.0f);?
	static const float PI2f = PIf * 2.0f;
	static const float RADIANSf = PI2f;
	static const float TO_RADf = PIf / 180.0f;
	static const float TO_DEGf = 180.0f / PIf;

	//static const float ϕf = (std::sqrtf(5.0f) - 1.0f) / 2.0f;
	static const float PHIf = 1.6180339887f; //(std::sqrtf(5.0f) - 1.0f) / 2.0f; // The golden ratio

	struct byte_offset
	{
		uint32_t offset;
		uint32_t size;
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

	struct RectXY
	{
		long x1, y1, x2, y2;
	};

	constexpr glm::vec3 circle_xz(float radius, float angle)
	{
		return glm::vec3(radius * std::cosf(angle), 0, radius * std::sinf(angle));
	}

	static const void* GLvoidptr(uint32_t offset)
	{
		// semantically correct!?
		const auto pointer = static_cast<uintptr_t>(offset);
		return reinterpret_cast<void*>(pointer);
	}
	
	//static const void* GLoffset_0 = GLvoidptr(0);
}