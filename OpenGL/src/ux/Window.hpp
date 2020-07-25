#pragma once

#include <GLFW/glfw3.h>


namespace ux {

    class Window {
    private:
        int windowWidth;
        int windowHeight;
        glm::ivec2 origWindowSize;
        glm::ivec2 origFramebufferSize;
        glm::ivec2 windowSize;
        int framebufferWidth;
        int framebufferHeight;
        glm::ivec2 framebufferSize;
        float aspectRatio;

        static void window_pos_callback(GLFWwindow* window, int xpos, int ypos)
        {
            //std::cout << "Window Pos: " << xpos << " " << ypos << std::endl;
            //windowPosition.x = xpos;
            //windowPosition.y = ypos;
        }

        static void window_size_callback(GLFWwindow* window, int width, int height)
        {
            std::cout << "Window Size: " << width << " " << height << std::endl;
            // swap the buffer when we resize in realtime!
            glfwSwapBuffers(window);
            //windowSize.x = width;
            //windowSize.y = height;
        }

        static void window_close_callback(GLFWwindow* window)
        {
            std::cout << "Closing window..." << std::endl;
            //if (!time_to_close)
            //    glfwSetWindowShouldClose(window, GLFW_FALSE);
        }

        static void window_content_scale_callback(GLFWwindow* window, float xscale, float yscale)
        {
            //set_interface_scale(xscale, yscale);
        }

        static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
        {
            std::cout << "Framebuffer Size: " << width << " " << height << std::endl;
            glViewport(0, 0, width, height);
            //ratio = (double)width / (double)height;
            //framebufferSize.x = width;
            //framebufferSize.y = height;
        }

        static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
        {
            //std::cout << xpos << "x" << ypos << std::endl;
        }

        static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
        {
            if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
            {
                //std::cout << "Clicked (LEFT)!" << std::endl;
            }
        }


    public:
        inline Window() : windowWidth(0), windowHeight(0), framebufferWidth(0), framebufferHeight(0), aspectRatio(0.0f) 
        {
            // Create a windowed mode window and its OpenGL context.
            origWindowSize = glm::ivec2(1500, 900);
            //origWindowSize = glm::ivec2(4800, 1000);
            windowSize = origWindowSize;
            GLFWwindow* window = glfwCreateWindow(origWindowSize.x, origWindowSize.y, "UX[GL]", NULL, NULL);
            if (!window)
            {
                std::cerr << "Something went wrong creating the window!" << std::endl;
                glfwTerminate();
                //return -1;
            }

            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

            // Set the window's callback function.
            glfwSetWindowPosCallback(window, window_pos_callback);
            glfwSetWindowSizeCallback(window, window_size_callback);
            glfwSetWindowCloseCallback(window, window_close_callback);
            glfwSetWindowContentScaleCallback(window, window_content_scale_callback);
            glfwSetCursorPosCallback(window, cursor_position_callback);
            glfwSetMouseButtonCallback(window, mouse_button_callback);
            glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
            // glfwSetWindowSizeLimits(window, 200, 200, 400, 400);
            // glfwSetWindowSizeLimits(window, 640, 480, GLFW_DONT_CARE, GLFW_DONT_CARE);
            // glfwSetWindowAspectRatio(window, 16, 9);
            //glfwGetWindowSize(window, &width, &height);
            //glfwSetWindowAspectRatio(window, width, height);
            int wwidth, wheight;
            glfwGetWindowSize(window, &wwidth, &wheight);
            glfwSetWindowAspectRatio(window, wwidth, wheight);


            // https://www.glfw.org/docs/3.3/window_guide.html#window_hints_wnd
            //glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
            //glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
            glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
            glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);

            // Make the window's context current.
            // IMPORTANT: has to be done before glewInit()
            glfwMakeContextCurrent(window);

            // GLEW init was here

            //glfwSwapInterval(1);
            glfwSwapInterval(0);

            origFramebufferSize = glm::ivec2(0);

            //int width, height;
            glfwGetFramebufferSize(window, &origFramebufferSize.x, &origFramebufferSize.y);
            framebufferSize = origFramebufferSize;
            aspectRatio = (float)framebufferSize.x / (float)framebufferSize.y;

            printf("Window Size: %d x %d\n", windowSize.x, windowSize.y);
            printf("Framebuffer Size: %d x %d\n", framebufferSize.x, framebufferSize.y);

            float xscale, yscale;
            glfwGetWindowContentScale(window, &xscale, &yscale);
            std::cout << "Window Content Scale: " << xscale << " " << yscale << std::endl;
        }

        inline void Draw() const
        {

        }
    };
}