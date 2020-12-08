#pragma once

#include <GLFW/glfw3.h>
#include <chrono>
#include <iostream>

using namespace std::literals;


namespace lux {
	/*
	LSUINT64 ui64CurTime = tTimer.GetCurMicros();
	while (ui64CurTime - ui64UpdatedTime > 33333ULL) {
		Update(33333ULL);
		ui64UpdatedTime += 33333ULL;
	}

	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	 std::time_t t_c = std::chrono::system_clock::to_time_t(now - std::chrono::hours(24));
	std::cout << "24 hours ago, the time was "
			  << std::put_time(std::localtime(&t_c), "%F %T") << '\n';

    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    std::time_t t_c = std::chrono::system_clock::to_time_t(now - std::chrono::hours(24));
    std::cout << "24 hours ago, the time was "
              << std::put_time(std::localtime(&t_c), "%F %T") << '\n';
 
    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
    std::cout << "Hello World\n";
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "Printing took "
              << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()
              << "us.\n";
	*/

    class Timer
    {
	public:
		Timer() : m_DeltaTime(0) {

			//m_LastTime = std::chrono::high_resolution_clock::now();
			//m_LastTime = std::chrono::system_clock::now();
			m_LastTime = glfwGetTime();
		}

		template<typename T>
		T GetSeconds() const { return static_cast<T>(m_DeltaTime); }

		template<typename T>
		T GetMilliseconds() const { return static_cast<T>(m_DeltaTime * 1000.0); }

		template<typename T>
		T GetTimestep() const {	return static_cast<T>(m_DeltaTime);	}

		template<typename T>
		operator T() const { return GetTimestep<T>(); }

		float CalcIncrement(const float& length, const float& duration) const
		{
			return length / (duration / GetTimestep<float>());
		}

		float PingPong(float& value, float& direction, const float& min, const float& max, const float& length, const float& duration) const
		{
			//value += direction * (length / (duration / m_DeltaTime));
			//clamp, floor
			value += direction * CalcIncrement(length, duration);
			if (value < min || value > max)
			{
				value = std::clamp<float>(value, min, max);
				direction = -direction;
			}
			/*
			if (value > max)
			{
				direction = -direction;
				value = max;
				return;
			}
			if (value < min)
			{
				direction = -direction;
				value = min;
				return;
			}
			*/
			return value;
		}


		void Update()
		{
			const auto currentTime = glfwGetTime();
			//auto currentTime = std::chrono::system_clock::now();
			//auto currentTime = std::chrono::high_resolution_clock::now();
			m_DeltaTime = currentTime - m_LastTime;
			m_LastTime = currentTime;			
		}

		/*
		//operator float() { return m_Time; }
		void UpdateSAVING_FOR_RESEARCH()
		{
			auto currentTime = glfwGetTime();
			//auto currentTime = std::chrono::system_clock::now();
			//auto currentTime = std::chrono::high_resolution_clock::now();
			auto difference = currentTime - m_LastTime;
			//auto int_ms = std::chrono::duration_cast<std::chrono::milliseconds>(difference);
			if (difference > m_Timestep)
			{
				std::cout << "Count: " << m_Count << std::endl;
				//std::cout << " Diff: " << difference.count() << std::endl;
				//std::cout << "   MS: " << int_ms.count() << std::endl;
				//std::cout << "   TS: " << m_Timestep.count() << std::endl;
				
				//UpdateScene(_33ms);

				m_LastTime += m_Timestep;
				

				//std::cout << "  CUR: " << currentTime.time_since_epoch().count() << std::endl;
				//std::cout << "  UPD: " << m_LastTime.time_since_epoch().count() << std::endl;
			}
			m_Count++;
		}
		*/
	private:
		double m_LastTime;
		//std::chrono::system_clock::time_point m_LastTime;
		//std::chrono::high_resolution_clock::time_point m_LastTime;
		// m_TicksPerSecond ...isn't that going to change?
		//std::chrono::milliseconds _33ms{ 33 };
		//std::chrono::milliseconds m_Timestep = 33ms;
		//double m_Timestep = 33333;
		//std::chrono::nanoseconds m_DeltaTime;
		double m_DeltaTime;
		//std::chrono::milliseconds m_DeltaTime;
		//uint64_t m_Count;
    };
}