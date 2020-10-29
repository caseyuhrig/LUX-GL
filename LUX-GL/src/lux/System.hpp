#pragma once

#include <vector>
#include "lux/Types.hpp"
#include "lux/Window.hpp"


/*
#ifdef __SSE__
UX_LOG_INFO("SSE (true)");
#else
UX_LOG_INFO("SSE (false)");
#endif
#ifdef __SSE2__
UX_LOG_INFO("SSE2 (true)");
#endif
#ifdef __SSE3__
UX_LOG_INFO("SSE3 (true)");
#endif
#ifdef __AVX__
UX_LOG_INFO("AVX (true)");
#endif
#ifdef __AVX2__
UX_LOG_INFO("AVX2 (true)");
#endif
*/

namespace lux {

    enum class MonitorPosition {
        Left = 0,
        Middle = 1,
        Right = 2
    };

    static class System
    {
    public:
        System()
        {
            m_Monitors = glfwGetMonitors(&m_MonitorCount);
            if (m_Monitors == NULL)
            {
                UX_LOG_FATAL("No monitors found or an error accured.");
                exit(1);
            }
            /*
            for (int n = 0;n < m_InterfaceCount;n++)
            {
                m_Interfaces[n] = CreateRef<Window>();
            }
            */
        }
        ~System() {
            delete[] m_Monitors;
        }
        //const int& GetMonitorCount() const { return m_MonitorCount; }

    private:
        int m_MonitorCount = -1;
        GLFWmonitor** m_Monitors;
        std::vector<Ref<Window>> m_Windows;
    };
}