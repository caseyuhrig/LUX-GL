#pragma once

#include <glad/glad.h>

#include <iostream>
#include <string>
#include <algorithm>
#include <memory>
#include <numbers>
//#include <ctime>

//#include <stdarg.h>
#include <glm/glm.hpp>


//#define ASSERT(x) if (!(x)) __debugbreak();

// #include <limits>
// const int min_int = std::numeric_limits<int>::min();
// const int max_int = std::numeric_limits<int>::max();


namespace lux {

	constexpr double PI = std::numbers::pi_v<double>; // std::acos(-1.0);
	constexpr double PI2 = PI * 2.0;
	constexpr double RADIANS = PI2;
	constexpr double TO_RAD = PI / 180.0;
	constexpr double TO_DEG = 180.0 / PI;
	
	constexpr float PIf = std::numbers::pi_v<float>;
	constexpr float PI2f = PIf * 2.0f;
	constexpr float RADIANSf = PI2f;
	constexpr float TO_RADf = PIf / 180.0f;
	constexpr float TO_DEGf = 180.0f / PIf;

	//template<typename T>
	//constexpr auto π = std::numbers::pi_v<T>;
	// the golden ratio Φ constant
	//template<typename T>
	//constexpr auto Φ = std::numbers::phi_v<T>;
	
	// 2² √ √ 
	//template<typename T>
	//constexpr auto √(T value) { return std::sqrt<T>(value); }

	//static const float ϕf = (std::sqrtf(5.0f) - 1.0f) / 2.0f;
	constexpr float PHIf = std::numbers::phi_v<float>; // The golden ratio


	struct byte_offset
	{
		uint32_t offset;
		size_t size;
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
	static const T random(const T max)
	{
		//return static_cast <T> (std::rand()) / static_cast <T> (std::RAND_MAX / max);
		return static_cast <T> (std::rand() / (RAND_MAX / max));
	}

	struct RectXY
	{
		long x1, y1, x2, y2;
	};

	constexpr glm::vec3 circle_xz(const float radius, const float angle)
	{
		return glm::vec3(radius * std::cosf(angle), 0, radius * std::sinf(angle));
	}

	// Don't like this, only using so I can find the places this is being done.
	inline static const auto make_void_ptr(uint32_t offset)
	{
		return reinterpret_cast<void*>(offset);
	}

}