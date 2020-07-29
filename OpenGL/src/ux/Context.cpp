#include "Context.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Log.hpp"



void ux::Context::Init()
{
    glfwMakeContextCurrent(_window_handle);    // <---- IMPORTANT: has to be done before glewInit()
    


    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        // Problem: glewInit failed, something is seriously wrong.
        std::cerr << "[ERROR]: " << glewGetErrorString(err) << std::endl;
    }
    UX_LOG_INFO("Status:   Using GL: %s", glGetString(GL_VERSION));
    UX_LOG_INFO("Status: Using GLEW: %s", glewGetString(GLEW_VERSION));

    //HZ_CORE_INFO("OpenGL Info:");
    //HZ_CORE_INFO("  Vendor: {0}", glGetString(GL_VENDOR));
    //HZ_CORE_INFO("  Renderer: {0}", glGetString(GL_RENDERER));
    //HZ_CORE_INFO("  Version: {0}", glGetString(GL_VERSION));

    int versionMajor;
    int versionMinor;
    glGetIntegerv(GL_MAJOR_VERSION, &versionMajor);
    glGetIntegerv(GL_MINOR_VERSION, &versionMinor);

    //HZ_CORE_ASSERT(versionMajor > 4 || (versionMajor == 4 && versionMinor >= 5), "Hazel requires at least OpenGL version 4.5!");


    // During init, enable debug output
    glEnable(GL_DEBUG_OUTPUT);
    //glDebugMessageCallback(ux::Window::message_callback, 0);
    glDebugMessageCallback([](unsigned int source, unsigned int type, unsigned int id,
        unsigned int severity, int length,
        const char* message, const void* userParam) {

            fprintf(stdout, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
                (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type, severity, message);

        }, 0);

    // context information
    // Uniform Buffer Objects (UBO)
    int max_uniform_buffer_bindings, max_uniform_block_size,
        max_vertex_uniform_blocks, max_fragment_uniform_blocks,
        max_geometry_uniform_blocks, gl_max_vertex_uniform_components;

    glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &max_uniform_buffer_bindings);
    glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &max_uniform_block_size);
    glGetIntegerv(GL_MAX_VERTEX_UNIFORM_BLOCKS, &max_vertex_uniform_blocks);
    glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_BLOCKS, &max_fragment_uniform_blocks);
    glGetIntegerv(GL_MAX_GEOMETRY_UNIFORM_BLOCKS, &max_geometry_uniform_blocks);
    glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS, &gl_max_vertex_uniform_components);

    std::cout << "GL_MAX_UNIFORM_BUFFER_BINDINGS: " << max_uniform_buffer_bindings << std::endl;
    std::cout << "GL_MAX_UNIFORM_BLOCK_SIZE: " << max_uniform_block_size << std::endl;
    std::cout << "GL_MAX_VERTEX_UNIFORM_BLOCKS: " << max_vertex_uniform_blocks << std::endl;
    std::cout << "GL_MAX_FRAGMENT_UNIFORM_BLOCKS: " << max_fragment_uniform_blocks << std::endl;
    std::cout << "GL_MAX_GEOMETRY_UNIFORM_BLOCKS : " << max_geometry_uniform_blocks << std::endl;


    //glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    if (true) // USE_ANTIALIASING
    {
        glEnable(GL_MULTISAMPLE);
    }
    // Make sure to turn backface culling off (glDisable(GL_CULL_FACE) ) because since we can look through the mesh, we could see that it has no “back” face.
    glDisable(GL_CULL_FACE);

}

void ux::Context::SwapBuffers() const
{
    glfwSwapBuffers(_window_handle);
}