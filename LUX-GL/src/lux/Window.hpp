#pragma once

#include <pch.hpp>

#include <lux/Context.hpp>
#include <lux/Types.hpp>
#include <lux/Platform/Microsoft/Windows.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace lux {

    class Window {
    private:
        GLFWwindow* _window_handle;
        //Scope<Context> _context;
        std::string title;
        int _window_width, _window_height;
        int _framebuffer_width, _framebuffer_height;
        bool m_vSync = false;
        std::vector<std::function<void(int width, int height)>> m_ResizeListeners;
    public:
        Window(const std::string& title, int window_width = 1280, int window_height = 720, GLFWwindow* share = nullptr)
            : _window_width(window_width), _window_height(window_height),
            _framebuffer_width(0), _framebuffer_height(0), _window_handle(nullptr)
        {
            if (!glfwInit())
            {
                spdlog::critical("Something went wrong initializing GLFW!");
                return;
            }
            else {
                spdlog::info("GLFW OK");
            }
            // https://www.glfw.org/docs/3.3/window_guide.html#window_hints_wnd
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
            //glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, false);
            // GLFW_SAMPLES doesn't do anything when using custom framebuffers.
            //if (USE_ANTIALIASING) glfwWindowHint(GLFW_SAMPLES, 8);
            //glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
            //glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
            //glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
            glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_FALSE);
            // does the cool transparent window thing.
            // WAY slow framerate though!
            //glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
            //glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);

            const auto* primaryMonitor = glfwGetPrimaryMonitor();

            int count;
            auto** monitors = glfwGetMonitors(&count);
            for (auto n = 0;n < count;n++)
            {
                auto* monitor = monitors[n];
                const auto* monitorName = glfwGetMonitorName(monitor);
                auto xpos = 0, ypos = 0;
                glfwGetMonitorPos(monitor, &xpos, &ypos);
                const auto* mode = glfwGetVideoMode(monitor);
                const auto primary = (monitor == primaryMonitor) ? true : false;
                const auto loc = (xpos < 0) ? 1 : (xpos > 0) ? 3 : 2;
                spdlog::info("Monitor: {} {}x{} Pos: {}x{} Pri: {} Loc: {}", 
                    monitorName, mode->width, mode->height,
                    xpos, ypos, primary ? "true" : "false", loc);
                    
                //const RectXY workArea = lux::Platform::Microsoft::Windows::GetWorkArea({ xpos,ypos,mode->width,mode->height });
                //UX_LOG_INFO("WorkArea: %d %d %d %d", workArea.x1, workArea.y1, workArea.x2, workArea.y2);
            }
            // Create a windowed mode window and its OpenGL context.
            _window_handle = glfwCreateWindow(window_width, window_height, title.c_str(), NULL, share);
            if (!_window_handle)
            {
                spdlog::critical("Something went wrong creating the window!");
                glfwTerminate();
                //return -1;
                return;
            }
            else {
                spdlog::info("WINDOW OK");
            }
            const std::string glfwVersion = glfwGetVersionString();
            int glfwMajorVersion, glfwMinorVersion, glfwRevision;
            glfwGetVersion(&glfwMajorVersion, &glfwMinorVersion, &glfwRevision);
            spdlog::info("      GLFW VERSION: {}", glfwVersion);
            spdlog::info("GLFW MAJOR VERSION: {}", glfwMajorVersion);
            spdlog::info("GLFW MINOR VERSION: {}", glfwMinorVersion);
            spdlog::info("     GLFW REVISION: {}", glfwRevision);

            GLFWimage icons[1];
            //stbi_set_flip_vertically_on_load(true);
            icons[0].pixels = stbi_load("res/images/ux-gl-sim-icon.png", &icons[0].width, &icons[0].height, 0, STBI_rgb_alpha);
            glfwSetWindowIcon(_window_handle, 1, icons);
            stbi_image_free(icons[0].pixels);

            glfwSetInputMode(_window_handle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

            //glfwSwapInterval(1); // same as set vsync = true
            glfwSwapInterval(m_vSync);

            glfwGetFramebufferSize(_window_handle, &_framebuffer_width, &_framebuffer_height);

            spdlog::info("Window Size: {} x {}", _window_width, _window_height);
            spdlog::info("Framebuffer Size: {} x {}", _framebuffer_width, _framebuffer_height);

            glfwSetWindowUserPointer(_window_handle, this);

            glfwSetWindowSizeCallback(_window_handle, [](GLFWwindow* window, int width, int height) //mutable
                {
                    auto* that = static_cast<Window*>(glfwGetWindowUserPointer(window));
                    that->_window_width = width;
                    that->_window_height = height;

                    // fire off any events here
                    spdlog::debug("Window Size Callback: {}x{}", width, height);

                    //glfwSetWindowAspectRatio(window, width, height);

                    // swap the buffer when we resize in realtime!
                    //glfwSwapBuffers(window);
                });
            glfwSetWindowCloseCallback(_window_handle, [](GLFWwindow* window) {});
            glfwSetWindowContentScaleCallback(_window_handle, [](GLFWwindow* window, float xscale, float yscale)
                {
                    //auto* _this = (Window*)glfwGetWindowUserPointer(window);
                    //_this->_xscale = xscale;
                    //_this->_yscale = yscale;
                    //std::cout << "WindowContentScaleCallback " << xscale << " " << yscale << std::endl;
                });
            glfwSetCursorPosCallback(_window_handle, [](GLFWwindow* window, double xpos, double ypos)
                {
                    //std::cout << "CursorPosCallback " << xpos << " " << ypos << std::endl;
                });
            glfwSetMouseButtonCallback(_window_handle, [](GLFWwindow* window, int button, int action, int mods)
                {
                    //std::cout << "MouseButtonCallback " << std::endl;
                    //UX_LOG_INFO("MouseButtonCallback");
                });
            glfwSetScrollCallback(_window_handle, [](GLFWwindow* window, double xoffset, double yoffset)
                {
                    //auto* _this = (Window*)glfwGetWindowUserPointer(window);
                    //UX_LOG_INFO("ScrollCallback: %f %f", xoffset, yoffset);
                });
            glfwSetKeyCallback(_window_handle, [](GLFWwindow* window, int key, int scancode, int action, int mods)
                {
                    //UX_LOG_DEBUG("Pressed: %d", key);
                });
            glfwSetFramebufferSizeCallback(_window_handle, [](GLFWwindow* window, int width, int height)
                {
                    auto* that = (Window*)glfwGetWindowUserPointer(window);
                    that->_framebuffer_width = width;
                    that->_framebuffer_height = height;
                    //glViewport(0, 0, width, height);
                    // TODO set the aspect ratio so the window doesn't get scewed.
                    //      Update the camera setting and whot-not.  Need to register
                    //      a listener from the main program.
                    spdlog::debug("Framebuffer Size Callback: {}x{}", width, height);

                    //if (_this->m_Camera)
                    //    _this->m_Camera->SetViewportSize(width, height);

                    for (auto resize : that->m_ResizeListeners)                  
                        resize(width, height);
                });

            // notified when its time to repaint the window contents
            // bet this can be used in combination with an event system to we don't have to loop
            // forever redrawing all the time!
            glfwSetWindowRefreshCallback(_window_handle, [](GLFWwindow* window)
                {
                    //auto* _this = (Window*)glfwGetWindowUserPointer(window);
                    //std::cout << "WindowRefreshCallback " << _this->_window_width << " " << _this->_window_height << std::endl;
                    // draw
                    //glfwSwapBuffers(window);
                });

            //glfwSetWindowFocusCallback
            //glfwSetWindowIconifyCallback


            //glfwSetWindowSizeLimits(window, 200, 200, 400, 400);
            //glfwSetWindowSizeLimits(window, 640, 480, GLFW_DONT_CARE, GLFW_DONT_CARE);
            //glfwSetWindowAspectRatio(window, 16, 9);
            //glfwGetWindowSize(window, &width, &height);
            //glfwSetWindowAspectRatio(window, width, height);   

            


            //_context = CreateScope<Context>(_window_handle);
            //_context->Init();
        }

        ~Window() = default;

        void MakeContextCurrent() const
        {
            glfwMakeContextCurrent(_window_handle);
        }

        void AddResizeListener(std::function<void(int width, int height)> callback)
        {
            m_ResizeListeners.push_back(callback);
        }       

        void FillWorkArea()
        {
            int xpos, ypos, width, height;
            glfwGetWindowPos(_window_handle, &xpos, &ypos);
            glfwGetWindowSize(_window_handle, &width, &height);
            const RectXY workArea = lux::Platform::Microsoft::Windows::GetWorkArea(_window_handle);
            const int titleBarHeight = lux::Platform::Microsoft::Windows::GetTitleBarHeight(_window_handle);
            spdlog::info("WorkArea: {} {} {} {}", workArea.x1, workArea.y1, workArea.x2, workArea.y2);
            spdlog::info("TitleBar Height: {}", titleBarHeight);
            _window_width = workArea.x2 - workArea.x1;
            _window_height = workArea.y2 - workArea.y1 - titleBarHeight;
            glfwSetWindowPos(_window_handle, workArea.x1, workArea.y1 + titleBarHeight);
            glfwSetWindowSize(_window_handle, _window_width, _window_height);
        }

        void Center()
        {
            auto* monitor = _getBestMonitor();
            if (!monitor) {
                spdlog::critical("A \"BEST\" monitor could not be found.");
                return;
            }
            const auto* mode = glfwGetVideoMode(monitor);
            if (!mode) {
                spdlog::critical("A video mode could not be found.");
                return;
            }
            int monitorX, monitorY;
            int windowWidth, windowHeight;
            glfwGetMonitorPos(monitor, &monitorX, &monitorY);
            glfwGetWindowSize(_window_handle, &windowWidth, &windowHeight);
            glfwSetWindowPos(_window_handle, monitorX + (mode->width - windowWidth) / 2, monitorY + (mode->height - windowHeight) / 2);
        }


        GLFWmonitor* _getBestMonitor()
        {
            int monitorCount;
            GLFWmonitor** monitors = glfwGetMonitors(&monitorCount);

            if (!monitors)
                return NULL;

            int windowX, windowY, windowWidth, windowHeight;
            glfwGetWindowSize(_window_handle, &windowWidth, &windowHeight);
            glfwGetWindowPos(_window_handle, &windowX, &windowY);

            GLFWmonitor* bestMonitor = nullptr;
            int bestArea = 0;

            for (int i = 0; i < monitorCount; ++i)
            {
                auto* monitor = monitors[i];

                int monitorX, monitorY;
                glfwGetMonitorPos(monitor, &monitorX, &monitorY);

                const auto* mode = glfwGetVideoMode(monitor);
                if (!mode)
                    continue;

                int areaMinX = std::max(windowX, monitorX);
                int areaMinY = std::max(windowY, monitorY);

                int areaMaxX = std::min(windowX + windowWidth, monitorX + mode->width);
                int areaMaxY = std::min(windowY + windowHeight, monitorY + mode->height);

                int area = (areaMaxX - areaMinX) * (areaMaxY - areaMinY);

                if (area > bestArea)
                {
                    bestArea = area;
                    bestMonitor = monitor;
                }
            }
            return bestMonitor;
        }

        GLFWwindow* GetGlfwWindow() const { return _window_handle; }
        /*
        const HWND GetNativeWindow() const {
            const HWND hwnd = glfwGetWin32Window(_window_handle);
            return hwnd;
        }
        */
        template<typename N = int> 
        N GetWidth() const {
            static_assert(std::is_arithmetic<N>::value, "Not an arithmetic type");
            return static_cast<N>(_window_width);
        };

        template<typename N = int>
        N GetHeight() const {
            static_assert(std::is_arithmetic<N>::value, "Not an arithmetic type");
            return static_cast<N>(_window_height);
        }

        int GetFramebufferWidth() const { return _framebuffer_width; }
        int GetFramebufferHeight() const { return _framebuffer_height; }
        //inline float GetAspectRatio() const { return _aspect_ratio; }

        void SetSize(const int width, const int height) const { glfwSetWindowSize(_window_handle, width, height); }

        /**
        * Fills all monitors, full screen, no borders!
        */
        void SuperSize() const
        {
            glfwSetWindowAttrib(_window_handle, GLFW_DECORATED, 0);
            glfwSetWindowAttrib(_window_handle, GLFW_FLOATING, 1);
            constexpr int width = 1920 + 1600 + 1600;
            constexpr int height = 1200;
            SetPosition(-1920, 0);
            SetSize(width, height);
        }
        //void SetDrawCallback();
        //inline void Draw(void) const {}

        glm::ivec2 GetPosition() const {
            glm::ivec2 position;
            glfwGetWindowPos(_window_handle, &position.x, &position.y);
            return position;
        }
        void SetPosition(const int x, const int y) const { glfwSetWindowPos(_window_handle, x, y); }
        bool ShouldClose() const { return glfwWindowShouldClose(_window_handle); }

        void Update() const
        {          
            glfwSwapBuffers(_window_handle);
            //_context->SwapBuffers();
            glfwPollEvents();
        }

        void Close() const { glfwSetWindowShouldClose(_window_handle, 1); }

        void Destroy() const { glfwDestroyWindow(_window_handle); }
    };
}