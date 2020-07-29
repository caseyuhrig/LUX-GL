#include "Window.hpp"


#include <iostream>
#include <cstdio>
#include "Log.hpp"
//#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"


ux::Window::Window(const std::string& title, int window_width = 1280, int window_height = 720)
    : _window_width(window_width), _window_height(window_height), _aspect_ratio(0),
    _framebuffer_width(0), _framebuffer_height(0), _xscale(0), _yscale(0), _window_handle(nullptr)
{
    static const bool USE_ANTIALIASING = true;

    if (!glfwInit())
    {
        std::cerr << "[ERROR] Something went wrong initializing GLFW!" << std::endl;
        return;
    }
    // https://www.glfw.org/docs/3.3/window_guide.html#window_hints_wnd
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    if (USE_ANTIALIASING)
    {
        glfwWindowHint(GLFW_SAMPLES, 8);
    }    
    //glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
    //glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);

    // does the cool transparent window thing.
    // WAY slow framerate though!
    //glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
    //glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);


    // Create a windowed mode window and its OpenGL context.
    _window_handle = glfwCreateWindow(window_width, window_height, title.c_str(), NULL, NULL);
    if (!_window_handle)
    {
        std::cout << "[ERROR] Something went wrong creating the window!" << std::endl;
        glfwTerminate();
        //return -1;
        return;
    }

    GLFWimage icons[1];
    //stbi_set_flip_vertically_on_load(true);
    icons[0].pixels = stbi_load("res/images/ux-gl-sim-icon.png", &icons[0].width, &icons[0].height, 0, STBI_rgb_alpha);
    glfwSetWindowIcon(_window_handle, 1, icons);
    stbi_image_free(icons[0].pixels);

    glfwSetInputMode(_window_handle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    glfwSetWindowUserPointer(_window_handle, this);

    glfwSetWindowSizeCallback(_window_handle, [](GLFWwindow* window, int width, int height)
    {
            auto* _this = (ux::Window*)glfwGetWindowUserPointer(window);
            _this->_window_width = width;
            _this->_window_height = height;
            // fire off any events here
            std::cout << "WindowSizeCallback: " << width << " " << height << std::endl;

            glfwSetWindowAspectRatio(window, width, height);

            // swap the buffer when we resize in realtime!
            glfwSwapBuffers(window);
    });
    glfwSetWindowCloseCallback(_window_handle, [](GLFWwindow* window) {});
    glfwSetWindowContentScaleCallback(_window_handle, [](GLFWwindow* window, float xscale, float yscale) 
    {
            auto* _this = (ux::Window*)glfwGetWindowUserPointer(window);
            _this->_xscale = xscale;
            _this->_yscale = yscale;
            std::cout << "WindowContentScaleCallback " << xscale << " " << yscale << std::endl;
    });
    glfwSetCursorPosCallback(_window_handle, [](GLFWwindow* window, double xpos, double ypos) 
    {
            //std::cout << "CursorPosCallback " << xpos << " " << ypos << std::endl;
    });
    glfwSetMouseButtonCallback(_window_handle, [](GLFWwindow* window, int button, int action, int mods) 
    {
            std::cout << "MouseButtonCallback " <<  std::endl;
            UX_LOG_INFO("MouseButtonCallback");
    });
    glfwSetScrollCallback(_window_handle, [](GLFWwindow * window, double xoffset, double yoffset)
    {
            auto* _this = (ux::Window*)glfwGetWindowUserPointer(window);
            UX_LOG_INFO("ScrollCallback: %f %f", xoffset, yoffset);
    });
    glfwSetKeyCallback(_window_handle, [](GLFWwindow* window, int key, int scancode, int action, int mods) 
    {
            UX_LOG_DEBUG("Pressed: %d", key);
    });
    glfwSetFramebufferSizeCallback(_window_handle, [](GLFWwindow* window, int width, int height)
    {
            auto* _this = (ux::Window*)glfwGetWindowUserPointer(window);
            _this->_framebuffer_width = width;
            _this->_framebuffer_height = height;
            glViewport(0, 0, width, height);
            // TODO set the aspect ratio so the window doesn't get scewed.
            std::cout << "FramebufferSizeCallback " << width << " " << height << std::endl;
    });

    // notified when its time to repaint the window contents
    // bet this can be used in combination with an event system to we don't have to loop
    // forever redrawing all the time!
    glfwSetWindowRefreshCallback(_window_handle, [](GLFWwindow* window) 
    {
            auto* _this = (ux::Window*)glfwGetWindowUserPointer(window);
            std::cout << "WindowRefreshCallback " << _this->_window_width << " " << _this->_window_height << std::endl;
            // draw
            glfwSwapBuffers(window);
    });

    //glfwSetWindowFocusCallback
    //glfwSetWindowIconifyCallback


    //glfwSetWindowSizeLimits(window, 200, 200, 400, 400);
    //glfwSetWindowSizeLimits(window, 640, 480, GLFW_DONT_CARE, GLFW_DONT_CARE);
    //glfwSetWindowAspectRatio(window, 16, 9);
    //glfwGetWindowSize(window, &width, &height);
    //glfwSetWindowAspectRatio(window, width, height);   

    bool _v_sync = false;
    //glfwSwapInterval(1); // same as set vsync = true
    glfwSwapInterval(_v_sync);

    glfwGetFramebufferSize(_window_handle, &_framebuffer_width, &_framebuffer_height);
    glfwGetWindowContentScale(_window_handle, &_xscale, &_yscale);
    _aspect_ratio = static_cast<float>(_framebuffer_width) / static_cast<float>(_framebuffer_height);

    UX_LOG_INFO("Window Size: %d x %d", _window_width, _window_height);
    UX_LOG_INFO("Framebuffer Size: %d x %d", _framebuffer_width, _framebuffer_height);
    UX_LOG_INFO("Framebuffer Aspect Ratio: %f", _aspect_ratio);
    UX_LOG_INFO("Window Content Scale: %f %f", _xscale, _yscale);

    // TODO Set the aspect ratio here?

    _context = CreateScope<Context>(_window_handle);
    _context->Init();
}

void ux::Window::Center()
{
    GLFWmonitor* monitor = _getBestMonitor();
    if (!monitor)
        return;

    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    if (!mode)
        return;

    int monitorX, monitorY;
    glfwGetMonitorPos(monitor, &monitorX, &monitorY);

    int windowWidth, windowHeight;
    glfwGetWindowSize(_window_handle, &windowWidth, &windowHeight);

    glfwSetWindowPos(_window_handle, monitorX + (mode->width - windowWidth) / 2, monitorY + (mode->height - windowHeight) / 2);
}


GLFWmonitor* ux::Window::_getBestMonitor()
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
        GLFWmonitor* monitor = monitors[i];

        int monitorX, monitorY;
        glfwGetMonitorPos(monitor, &monitorX, &monitorY);

        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
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


/*
void ux::Window::window_pos_callback(GLFWwindow* window, int xpos, int ypos)
{
    //std::cout << "Window Pos: " << xpos << " " << ypos << std::endl;
    //windowPosition.x = xpos;
    //windowPosition.y = ypos;
}

void ux::Window::window_size_callback(GLFWwindow* window, int width, int height)
{
    std::cout << "Window Size: " << width << " " << height << std::endl;
    // swap the buffer when we resize in realtime!
    glfwSwapBuffers(window);
    //windowSize.x = width;
    //windowSize.y = height;
}

void ux::Window::window_close_callback(GLFWwindow* window)
{
    std::cout << "Closing window..." << std::endl;
    //if (!time_to_close)
    //    glfwSetWindowShouldClose(window, GLFW_FALSE);
}

void ux::Window::window_content_scale_callback(GLFWwindow* window, float xscale, float yscale)
{
    //set_interface_scale(xscale, yscale);
}


void ux::Window::framebuffer_size_callback(GLFWwindow* window, int framebuffer_width, int framebuffer_height)
{
    std::cout << "Framebuffer Size: " << framebuffer_width << " " << framebuffer_height << std::endl;
    //glViewport(0, 0, width, height);
    //ratio = (double)width / (double)height;
    //framebufferSize.x = width;
    //framebufferSize.y = height;
    _framebuffer_width = framebuffer_width;
    _framebuffer_height = framebuffer_height;
}


void ux::Window::cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    //std::cout << xpos << "x" << ypos << std::endl;
}

void ux::Window::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        //std::cout << "Clicked (LEFT)!" << std::endl;
    }
}
*/

// TODO put in a context wrapper?
/*
void GLAPIENTRY ux::Window::message_callback(unsigned int source, unsigned int type, unsigned int id,
                                                unsigned int severity, int length,
                                                const char* message, const void* userParam)
{
    fprintf(stdout, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
        (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type, severity, message);
}
*/

