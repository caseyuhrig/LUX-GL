#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "Context.hpp"
#include "Core.hpp"

/*
   HOW TO SETUP Modern OpenGl!

   GLFW and GLEW have to be setup.

   Where does the glew32.dll go?  Doesn't, link against the glew32s.lib

   VERY IMPORTANT: In preferences: C/C++ -> Preprocessor -> Preprocessor Definitions (add) GLEW_STATIC in the ; delimited list!

   GL/glew.h contains the main glMethods(...)

   Link to all the *.lib files as needed and include all the header *.h files.
*/


namespace ux {

    class Window {
    private:
        GLFWwindow* _window_handle;
        Scope<Context> _context;
        std::string title;
        int _window_width, _window_height;
        int _framebuffer_width, _framebuffer_height;
        float _aspect_ratio;
        float _xscale, _yscale;

        GLFWmonitor* _getBestMonitor();
    public:
      
        Window(const std::string& title, int window_width, int window_height);
        //~Window() = default;
        //void InitContext();

        inline inline GLFWwindow* GetNativeWindow() const { return _window_handle; }

        // Still getting myself up to speed with C++, so making notes.

        template<typename N = int>                           // defaults to int return type
        inline N GetWidth() const {                          // go read about const in class member methods.
            static_assert(std::is_arithmetic<N>::value,      // assert checked at compile time
                "Not an arithmetic type");
            return static_cast<N>(_window_width);
        };

        template<typename N = int>
        inline N GetHeight() const {
            static_assert(std::is_arithmetic<N>::value, 
                "Not an arithmetic type");
            return static_cast<N>(_window_height);
        }

        inline int GetFramebufferWidth() const { return _framebuffer_width; }

        inline int GetFramebufferHeight() const { return _framebuffer_height; }

        inline float GetAspectRatio() const { return _aspect_ratio; }

        void Center();

        //void SetDrawCallback();

        inline void Draw(void) const
        {

        }

        inline bool Loop() const { return !glfwWindowShouldClose(_window_handle); }

        inline void ContinueLoop() const
        {          
            _context->SwapBuffers();
            glfwPollEvents();
        }

        // can this be in the destructor?
        inline void Close() const
        {
            glfwDestroyWindow(_window_handle);
            glfwTerminate();
        }
    };
}