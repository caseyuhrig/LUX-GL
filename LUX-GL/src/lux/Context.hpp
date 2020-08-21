#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Log.hpp"


#define UX_ENABLE_DEBUG


namespace lux {

    class Context
    {
    private:
        GLFWwindow* _window_handle;
    public:
        Context(GLFWwindow* window_handle) : _window_handle(window_handle) {}

        void Init()
        {
            glfwMakeContextCurrent(_window_handle);    // <---- IMPORTANT: has to be done before glewInit()
            if (_window_handle == NULL)
            {
                UX_LOG_FATAL("Failed to create GLFW window");
                glfwTerminate();
            }


            //GLenum err = glewInit();
            //if (GLEW_OK != err)
            //{
                // Problem: glewInit failed, something is seriously wrong.
            //    std::cerr << "[ERROR]: " << glewGetErrorString(err) << std::endl;
            //}
            if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
            {
                UX_LOG_FATAL("Failed to initialize OpenGL context");
                //return -1;
                glfwTerminate();
            }
            UX_LOG_INFO("     Using OpenGL: %s", glGetString(GL_VERSION));
            //UX_LOG_INFO("Status: Using GLEW: %s", glewGetString(GLEW_VERSION));

     
            std::string glVendorString = (char*)glGetString(GL_VENDOR);
            std::string glVersionString = (char*)glGetString(GL_VERSION);
            std::string glRendererString = (char*)glGetString(GL_RENDERER);
            //std::string glewVersionString = std::string((char*)glewGetString(GLEW_VERSION));
            int version;
            int versionMajor;
            int versionMinor;
            glGetIntegerv(GL_VERSION, &version);
            glGetIntegerv(GL_MAJOR_VERSION, &versionMajor);
            glGetIntegerv(GL_MINOR_VERSION, &versionMinor);

            UX_LOG_INFO("  Vendor: %s", glVendorString);
            UX_LOG_INFO("Renderer: %s", glRendererString);
            UX_LOG_INFO(" Version: %s", glVersionString);

            //HZ_CORE_INFO("OpenGL Info:");
            //HZ_CORE_INFO("  Vendor: {0}", glGetString(GL_VENDOR));
            //HZ_CORE_INFO("  Renderer: {0}", glGetString(GL_RENDERER));
            //HZ_CORE_INFO("  Version: {0}", glGetString(GL_VERSION));

            //HZ_CORE_ASSERT(versionMajor > 4 || (versionMajor == 4 && versionMinor >= 5), "Hazel requires at least OpenGL version 4.5!");
            if (versionMajor < 4 || (versionMajor == 4 && versionMinor < 5))
            {
                UX_LOG_ERROR("UX requires at least OpenGL version 4.5!");
            }
            glDisable(GL_DEBUG_OUTPUT);
#ifdef UX_ENABLE_DEBUG
            // During init, enable debug output
            glEnable(GL_DEBUG_OUTPUT);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            //glDebugMessageCallback(OpenGLMessageCallback, nullptr);

            //glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
            //glDebugMessageCallback(ux::Window::message_callback, 0);
            glDebugMessageCallback([](unsigned int source, unsigned int type, unsigned int id,
                unsigned int severity, int length,
                const char* message, const void* userParam)
                {
                    //fprintf(stdout, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
                    //    (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type, severity, message);

                    // ignore non-significant error/warning codes
                    //if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

                    std::cout << "---------------" << std::endl;
                    std::cout << "Debug message (" << id << "): " << message << std::endl;

                    switch (source)
                    {
                    case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
                    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
                    case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
                    case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
                    case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
                    case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
                    } std::cout << std::endl;

                    switch (type)
                    {
                    case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
                    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
                    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break;
                    case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
                    case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
                    case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
                    case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
                    case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
                    case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
                    } std::cout << std::endl;

                    switch (severity)
                    {
                    case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
                    case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
                    case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
                    case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
                    } std::cout << std::endl;
                    std::cout << std::endl;
                }, 0);
            //glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_TRUE);
            glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
#endif
            // Uniform Buffer Object (UBO)
            int max_uniform_buffer_bindings, max_uniform_block_size,
                max_vertex_uniform_blocks, max_fragment_uniform_blocks,
                max_geometry_uniform_blocks, gl_max_vertex_uniform_components;

            glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &max_uniform_buffer_bindings);
            glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &max_uniform_block_size);
            glGetIntegerv(GL_MAX_VERTEX_UNIFORM_BLOCKS, &max_vertex_uniform_blocks);
            glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_BLOCKS, &max_fragment_uniform_blocks);
            glGetIntegerv(GL_MAX_GEOMETRY_UNIFORM_BLOCKS, &max_geometry_uniform_blocks);
            glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS, &gl_max_vertex_uniform_components);

            UX_LOG_INFO("GL_MAX_UNIFORM_BUFFER_BINDINGS: %d", max_uniform_buffer_bindings);
            UX_LOG_INFO("GL_MAX_UNIFORM_BUFFER_BINDINGS: %d", max_uniform_buffer_bindings);
            UX_LOG_INFO("     GL_MAX_UNIFORM_BLOCK_SIZE: %d", max_uniform_block_size);
            UX_LOG_INFO("  GL_MAX_VERTEX_UNIFORM_BLOCKS: %d", max_vertex_uniform_blocks);
            UX_LOG_INFO("GL_MAX_FRAGMENT_UNIFORM_BLOCKS: %d", max_fragment_uniform_blocks);
            UX_LOG_INFO("GL_MAX_GEOMETRY_UNIFORM_BLOCKS: %d", max_geometry_uniform_blocks);
            // Shader Storage Buffer Object (SSBO)
            int gl_max_shader_storage_buffer_bindings;

            glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &gl_max_shader_storage_buffer_bindings);

            UX_LOG_INFO("GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS: %d", gl_max_shader_storage_buffer_bindings);
                //GL_MAX_SHADER_STORAGE_BLOCK_SIZE = 16777216
                //GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS = 16
                //GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS = 16
                //GL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS = 16
                //GL_MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS = 16
                //GL_MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS = 16
                //GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS = 16
                //GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS = 16

            //const GLubyte* sExtensions = glGetString(GL_EXTENSIONS);

            //std::cout << sExtensions << std::endl;
        }

        void SwapBuffers() const
        {
            glfwSwapBuffers(_window_handle);
        }
    };
}