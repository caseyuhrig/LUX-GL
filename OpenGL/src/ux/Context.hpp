#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>


namespace ux {

    class Context
    {
    private:
        GLFWwindow* _window_handle;
    public:
        Context(GLFWwindow* window_handle) : _window_handle(window_handle) {}

        void Init();
        void SwapBuffers() const;
    };
}