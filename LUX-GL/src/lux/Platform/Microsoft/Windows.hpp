#pragma once

#include <windows.h>

#include "lux/Log.hpp"
#include "lux/Types.hpp"

#define GLFW_EXPOSE_NATIVE_WGL
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>



namespace lux {
    namespace Platform {
        namespace Microsoft {

            class Windows
            {
            public:
                Windows() {}
                /*
                static constexpr RectXY GetWorkArea(const HWND& hwnd)
                {
                    const HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
                        
                    
                }
                */
                /*
                static const RectXY GetWorkArea(const RectXY& windowRect)
                {
                    RECT rect;
                    rect.left = windowRect.x1;
                    rect.right = windowRect.x2;
                    rect.top = windowRect.y1;
                    rect.bottom = windowRect.y2;

                    const HMONITOR hMonitor = MonitorFromRect(&rect, MONITOR_DEFAULTTONEAREST);

                    MONITORINFO mi;
                    mi.cbSize = sizeof(mi);

                    if (GetMonitorInfo(hMonitor, &mi) == 0)
                    {
                        UX_LOG_ERROR("GetMonitorInfo");
                    }
                    return { mi.rcWork.left, mi.rcWork.top, mi.rcWork.right, mi.rcWork.bottom };
                }
                */
                static const RectXY GetWorkArea(GLFWwindow* window)
                {
                    const HWND hwnd = glfwGetWin32Window(window);
                    const HMONITOR hMonitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);

                    MONITORINFO mi;
                    mi.cbSize = sizeof(mi);

                    if (GetMonitorInfo(hMonitor, &mi) == 0)
                    {
                        spdlog::error("GetMonitorInfo");
                    }
                    return { mi.rcWork.left, mi.rcWork.top, mi.rcWork.right, mi.rcWork.bottom };
                }

                static const int GetTitleBarHeight(GLFWwindow* window)
                {
                    const HWND hwnd = glfwGetWin32Window(window);

                    TITLEBARINFOEX ptinfo;// = (TITLEBARINFOEX*)malloc(sizeof(TITLEBARINFOEX));
                    ptinfo.cbSize = sizeof(TITLEBARINFOEX);
                    SendMessage(hwnd, WM_GETTITLEBARINFOEX, 0, (LPARAM)&ptinfo);
                    int height = ptinfo.rcTitleBar.bottom - ptinfo.rcTitleBar.top;
                    int width = ptinfo.rcTitleBar.right - ptinfo.rcTitleBar.left;
                    //free(ptinfo);

                    return height;

                    RECT wrect;
                    GetWindowRect(hwnd, &wrect);
                    RECT crect;
                    GetClientRect(hwnd, &crect);

                    spdlog::info("win rect: {} {} {} {}", wrect.left, wrect.top, wrect.right, wrect.bottom);
                    spdlog::info("client rect: {} {} {} {}", crect.left, crect.top, crect.right, crect.bottom);

                    POINT lefttop = { crect.left, crect.top }; // Practicaly both are 0
                    ClientToScreen(hwnd, &lefttop);
                    POINT rightbottom = { crect.right, crect.bottom };
                    ClientToScreen(hwnd, &rightbottom);
                    spdlog::info("title: {} {} {} {}", lefttop.x, lefttop.y, rightbottom.x, rightbottom.y);
                    return rightbottom.y - lefttop.y;
                }
            private:

            };
        }
    }
}