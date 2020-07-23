/*
   HOW TO SETUP Modern OpenGl!

   GLFW and GLEW have to be setup.

   Where does the glew32.dll go?  Doesn't, link against the glew32s.lib

   VERY IMPORTANT: In preferences: C/C++ -> Preprocessor -> Preprocessor Definitions (add) GLEW_STATIC in the ; delimited list!

   GL/glew.h contains the main glMethods(...)

   Link to all the *.lib files as needed and include all the header *.h files.

   1) READ THE DOCUMENTATION ON THE LIBRARIES
   2) KNOW YOUR IDE AND HOW TO SET IT UP
*/



#include <GL/glew.h>        // Cross platform link between your graphics card and OpenGL.
#include <GLFW/glfw3.h>

//#define GLM_FORCE_INLINE
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <chrono>
#include <thread>
#include <ctime>

#include "ux/ux.h"

#include "glfont/GLFont.h";
#include "glfont/FTLabel.h";

#include "OBJ_Loader.h"

#include "UniformBuffer.hpp"


#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "entt/entt.hpp"

#include <irrklang/irrKlang.h>



glm::ivec2 origWindowSize, windowSize;
glm::ivec2 windowPosition;

glm::ivec2 origFramebufferSize, framebufferSize;
float ratio;


void window_pos_callback(GLFWwindow* window, int xpos, int ypos)
{
    //std::cout << "Window Pos: " << xpos << " " << ypos << std::endl;
    windowPosition.x = xpos;
    windowPosition.y = ypos;
}

void window_size_callback(GLFWwindow* window, int width, int height)
{
    std::cout << "Window Size: " << width << " " << height << std::endl;
    // swap the buffer when we resize in realtime!
    glfwSwapBuffers(window);
    windowSize.x = width;
    windowSize.y = height;
}

void window_close_callback(GLFWwindow* window)
{
    std::cout << "Closing window..." << std::endl;
    //if (!time_to_close)
    //    glfwSetWindowShouldClose(window, GLFW_FALSE);
}

void window_content_scale_callback(GLFWwindow* window, float xscale, float yscale)
{
    //set_interface_scale(xscale, yscale);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    std::cout << "Framebuffer Size: " << width << " " << height << std::endl;
    glViewport(0, 0, width, height);
    //ratio = (double)width / (double)height;
    framebufferSize.x = width;
    framebufferSize.y = height;
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    //std::cout << xpos << "x" << ypos << std::endl;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        //std::cout << "Clicked (LEFT)!" << std::endl;
    }
}


void GLAPIENTRY MessageCallback(unsigned int source, unsigned int type, unsigned int id,
    unsigned int severity, int length,
    const char* message, const void* userParam)
{
    fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
        (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type, severity, message);
}


glm::mat4 rotate(glm::mat4 model, glm::vec3 axis, glm::vec3 angle)
{
    //glm::mat4 rotMat = glm::rotate(glm::mat4(1.0), angle.x, glm::vec3(1.0, 0.0, 0.0));
    //rotMat = glm::rotate(rotMat, angle.y, glm::vec3(0.0, 1.0, 0.0));
    //rotMat = glm::rotate(rotMat, angle.z, glm::vec3(0.0, 0.0, 1.0));
    //rotMat = glm::translate(rotMat, axis);
    //return rotMat * model;
    return glm::translate(
        glm::rotate(
            glm::rotate(
                glm::rotate(glm::mat4(1.0),
                    angle.x, glm::vec3(1.0, 0.0, 0.0)),
                angle.y, glm::vec3(0.0, 1.0, 0.0)),
            angle.z, glm::vec3(0.0, 0.0, 1.0)),
        axis) * model;
}





int main(int argc, char** argv)
{
    static const bool USE_ANTIALIASING = true;

    // Initialize the library
    if (!glfwInit())
    {
        std::cerr << "Something went wrong initializing GLFW!" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    if (USE_ANTIALIASING)
    {
        glfwWindowHint(GLFW_SAMPLES, 8);
    }

    // Create a windowed mode window and its OpenGL context.
    origWindowSize = glm::ivec2(1500, 900);
    //origWindowSize = glm::ivec2(4800, 1000);
    windowSize = origWindowSize;
    GLFWwindow* window = glfwCreateWindow(origWindowSize.x, origWindowSize.y, "UX[GL]", NULL, NULL);
    if (!window)
    {
        std::cerr << "Something went wrong creating the window!" << std::endl;
        glfwTerminate();
        return -1;
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

    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        // Problem: glewInit failed, something is seriously wrong.
        std::cerr << "Error: " << glewGetErrorString(err) << std::endl;
    }
    std::cout << "Status:   Using GL: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "Status: Using GLEW: " << glewGetString(GLEW_VERSION) << std::endl;

    // During init, enable debug output
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);

    //glfwSwapInterval(1);
    glfwSwapInterval(0);

    //int width, height;
    glfwGetFramebufferSize(window, &origFramebufferSize.x, &origFramebufferSize.y);
    framebufferSize = origFramebufferSize;
    ratio = (float)framebufferSize.x / (float)framebufferSize.y;

    printf("Window Size: %d x %d\n", windowSize.x, windowSize.y);
    printf("Framebuffer Size: %d x %d\n", framebufferSize.x, framebufferSize.y);

    float xscale, yscale;
    glfwGetWindowContentScale(window, &xscale, &yscale);
    std::cout << "Window Content Scale: " << xscale << " " << yscale << std::endl;

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
    if (USE_ANTIALIASING)
    {
        glEnable(GL_MULTISAMPLE);
    }
    // Make sure to turn backface culling off (glDisable(GL_CULL_FACE) ) because since we can look through the mesh, we could see that it has no “back” face.
    glDisable(GL_CULL_FACE);

    // https://learnopengl.com/Advanced-OpenGL/Advanced-GLSL

    // Vertex shader variables
    //glEnable(GL_PROGRAM_POINT_SIZE);
    // gl_PointSize = 1 in shader for example
    // can be sent in a vertex buffer object

    // The integer variable gl_VertexID holds the current ID of the vertex we're drawing. When doing indexed rendering (with 
    // glDrawElements) this variable holds the current index of the vertex we're drawing. When drawing without indices (via 
    // glDrawArrays) this variable holds the number of the currently processed vertex since the start of the render call.

    // Fragment shader variables
    // gl_FragCoord, gl_FrontFacing, gl_FragDepth



    auto cube = ux::Cube(glm::vec3(1.0f));

    auto segment = ux::Segment(0.5f, 1.0f, 0.0f, 45.0f, 0.125f);

    auto segment2 = ux::Segment(1.25f, 1.5f, 0.0f, 15.0f, 0.125f);

    float color_front[4] = { 1.0f, 1.0f, 1.0f, 1.0f };







    auto shader = Shader("res/shaders/basic.shader");

    //auto texture = Texture("res/textures/cartographer-logo-600x600.png");
    //texture.Bind();
    //shader.SetUniform1i("u_Texture", 0);

    auto shaderQuad = Shader("res/shaders/quad.shader");

    auto shaderBox = Shader("res/shaders/box.shader");

    auto shaderBase = Shader("res/shaders/base.shader");

    auto shaderText = Shader("res/shaders/text.shader");

    auto shaderUX = Shader("res/shaders/ux.shader");



    glm::vec3 axis = { 0.0, 0.0, 0.0 };

    // all object in the scene are scaled by scale
    float scale = 25.0; //1.0; //0.125;


    glm::vec3 translate = { -1.4, 0.0, 0.0 };

    glm::vec3 mRotate = { 0.5, 0.0, 2.0 };
    float zNear = 0.01f;
    float zFar = 2000.0;

    float rotator = 0.0;
    float rotator_increment = 0.0; //0.001;

    // lighting
    glm::vec3 lightPos = glm::vec3(-24.0, 1.0, 1.0);
    glm::vec3 lightInc = glm::vec3(0.045, 0.0, 0.0);


    float translateZ = -100.0;
    glm::vec2 rotateXY = glm::vec2(0.0, glm::radians(0.0));


    glm::vec4 color = { 0.2, 0.2, 0.2, 1.0 };
    glm::vec4 light_color = { 1.0, 1.0, 1.0, 1.0 };
    glm::vec4 light_ambient = { 0.2, 0.2, 0.2, 1.0 };
    glm::vec4 light_diffuse = { 0.5, 0.5, 0.5, 1.0 };
    glm::vec4 light_specular = { 0.2, 0.2, 0.2, 1.0 };

    glm::vec3 camera_position = { 0.0, 0.0, 10.0 };
    glm::vec3 camera_look_at = { 0.0, 0.0, 0.0 };

    float gamma = 1.0;
    bool animate = true;
    bool show_text = true;

    //float ambient_strength = 0.1;
    //float specular_strength = 0.5;
    float specular_shininess = 32.0;

    glm::vec4 ambient_color = { 0.348f, 0.348f, 0.348f, 1.0f };
    glm::vec4 diffuse_color = { 0.608f, 0.608f, 0.608f, 1.0f };
    glm::vec4 specular_color = { 0.5f, 0.5f, 0.5f, 1.0f };


    glm::mat4 proj = glm::perspective(glm::radians(55.0f), (float)ratio, zNear, zFar); // PROJECTION (SCREEN)

    glm::mat4 viewBase = glm::lookAt(camera_position,     // Camera position in world space
        camera_look_at,      // look at origin
        glm::vec3(0, 1, 0)); // Head is up (set to 0, -1, 0 to look upside down)

    glm::mat4 viewTranslate = glm::translate(viewBase, glm::vec3(0.0, 0.0, translateZ));
    glm::mat4 viewRotateX = glm::rotate(viewTranslate, rotateXY.x, glm::vec3(1.0, 0.0, 0.0));
    glm::mat4 view = glm::rotate(viewRotateX, rotateXY.y, glm::vec3(0.0, 1.0, 0.0)); // CAMERA

    // If w == 0, the value stored is a vector, if w == 1, the value stored is a point. 

    glm::vec4 cp = glm::vec4(camera_position, 1.0);
    glm::vec4 cla = glm::vec4(camera_look_at, 1.0);
    //glm::vec4 lp = glm::vec4(lightPos, 1.0);

    typedef struct Awesome
    {
        glm::vec4      ux_color;               // 16
        // material
        glm::vec4      ux_ambient_color;       // 16
        glm::vec4      ux_diffuse_color;       // 16
        glm::vec4      ux_specular_color;      // 16
        float          ux_specular_shininess;  // 16 (4)
        float          padding1[3];
        // camera
        glm::vec4      ux_camera_position;     // 16
        glm::vec4      ux_camera_look_at;      // 16
        // light
        glm::vec4      ux_light_pos;           // 16 (12)
        //float          padding2[1];
        glm::vec4      ux_light_color;         // 16
        glm::vec4      ux_light_ambient;       // 16
        glm::vec4      ux_light_diffuse;       // 16
        glm::vec4      ux_light_specular;      // 16
        // core matrix(s)
        glm::mat4      ux_view;                // 64
        glm::mat4      ux_proj;                // 64
        // post-production
        float          ux_gamma;               // 16 (4)
        float          padding3[3];
    };

    typedef struct Light
    {

    };

    typedef struct Super
    {
        glm::vec4 s_color1;
        glm::vec4 s_color2;
    };

    typedef struct Pooper
    {
        glm::mat4 view;
        glm::mat4 proj;
    };

    Awesome awesome = {
        //                 (shader var name)         (type) (off) (sz)
        color,              // ux_color               vec4   0     16
        // material
        ambient_color,      // ux_ambient_color       vec4   16    16
        diffuse_color,      // ux_diffuse_color       vec4   32    16
        specular_color,     // ux_specular_color      vec4   48    16
        specular_shininess, // ux_specular_shininess  float  64    16  (4)
        {0.0f, 0.0f, 0.0f },
        // camera
        cp,                 // ux_camera_position     vec3   80    16
        cla,                // ux_camera_look_at      vec3   96    16
        // light
        glm::vec4(lightPos, 1.0),           // ux_light_pos           vec3   112   16  (12)
       // {0.0f},
        light_color,        // ux_light_color         vec4   128   16
        light_ambient,      // ux_light_ambient       vec4   144   16
        light_diffuse,      // ux_light_diffuse       vec4   160   16
        light_specular,     // ux_light_specular      vec4   176   16
        // core matrix(s)
        view,               // ux_view;               mat4   192   64
        proj,               // ux_proj;               mat4   256   64
        // post-production
        gamma,              // ux_gamma;              float  320   16  (4)
        {0.0f, 0.0f, 0.0f}
        //                                            (size)      336
    };

    
    

    Super super = {
        glm::vec4(1.0, 0.0, 0.0, 1.0),
        glm::vec4(0.0, 0.0, 1.0, 1.0)
    };

    Pooper pooper = { view, proj };


    auto ubo_Awesome = UniformBuffer("Awesome", 0, 336, &awesome);
    ubo_Awesome.AddUniform("ux_color", 0, 16);
    ubo_Awesome.AddUniform("ux_ambient_color", 16, 16);
    ubo_Awesome.AddUniform("ux_diffuse_color", 32, 16);
    ubo_Awesome.AddUniform("ux_specular_color", 48, 16);
    ubo_Awesome.AddUniform("ux_specular_shininess", 64, 16);
    ubo_Awesome.AddUniform("ux_camera_position", 80, 16);
    ubo_Awesome.AddUniform("ux_camera_look_at", 96, 16);
    ubo_Awesome.AddUniform("ux_light_pos", 112, 16);
    ubo_Awesome.AddUniform("ux_light_color", 128, 16);
    ubo_Awesome.AddUniform("ux_light_ambient", 144, 16);
    ubo_Awesome.AddUniform("ux_light_diffuse", 160, 16);
    ubo_Awesome.AddUniform("ux_light_specular", 176, 16);
    ubo_Awesome.AddUniform("ux_view", 192, 64);
    ubo_Awesome.AddUniform("ux_proj", 256, 64);
    ubo_Awesome.AddUniform("ux_gamma", 320, 16);

    ubo_Awesome.SetUniform("ux_ambient_color", ambient_color);
    ubo_Awesome.SetUniform("ux_light_diffuse", light_diffuse);
    ubo_Awesome.SetUniform("ux_light_specular", light_specular);
    ubo_Awesome.SetUniform("ux_view", view);
    //ubo_Awesome.SetUniform("ux_diffuse_color", diffuse_color);


    auto ubo_Super = UniformBuffer("Super", 2, sizeof(super), &super);
    ubo_Super.AddUniform("s_color1", 0, 16);
    ubo_Super.AddUniform("s_color2", 16, 16);
    ubo_Super.SetUniform("s_color2", glm::vec4(0, 1, 0, 1));
    //ubo_Super.SetUniform("s_color2", glm::vec4(0, 0, 1, 1));
    




    

    auto ubo_Pooper = UniformBuffer("Pooper", 1, sizeof(pooper), &pooper);
    ubo_Pooper.AddUniform("view", 0, 64);
    ubo_Pooper.AddUniform("proj", 64, 64);
    
    ubo_Pooper.SetUniform("view", view);
    ubo_Pooper.SetUniform("proj99", proj);
    
    



    Renderer renderer;

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    const char* glsl_version = "#version 440";
    ImGui::CreateContext();
    //ImGuiIO& io = ImGui::GetIO(); (void)io;
    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();


    



    auto lightCube = ux::Cube(glm::vec3(1.0f, 1.0f, 1.0f));



    auto lines = ux::Lines();
    //lines.add(glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    //lines.add(glm::vec3(0.5, 0, 0), glm::vec3(0.5, 1, 0));
    lines.add(glm::vec3(0.0f, framebufferSize.y - 1, 0.0f), glm::vec3(framebufferSize.x - 1, framebufferSize.y - 1, 0.0f)); // TOP
    lines.add(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(framebufferSize.x - 1, 0.0f, 0.0f)); // BOTTOM
    lines.add(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, framebufferSize.y - 1, 0.0f)); // LEFT
    lines.add(glm::vec3(framebufferSize.x - 1, 0.0f, 0.0f), glm::vec3(framebufferSize.x - 1, framebufferSize.y - 1, 0.0f)); // RIGHT
    lines.commit();

    auto lines2 = ux::Lines();
    lines2.createCircle(2.0, 360);

    auto ring = ux::Lines();
    ring.createRing(2.2, 2.3, 0.1, 180);

    auto triangles = ux::Triangles();
    triangles.createRing(2.2, 2.3, 0.1, 180);

 

    std::shared_ptr<GLFont> header_font = std::shared_ptr<GLFont>(new GLFont("res/fonts/Inconsolata/static/InconsolataCondensed-Medium.ttf"));
    std::shared_ptr<GLFont> glfont = std::shared_ptr<GLFont>(new GLFont("res/fonts/Inconsolata/static/InconsolataCondensed-Light.ttf"));

    int startX = 50;
    int startY = 50;



    std::unique_ptr<FTLabel> header = std::unique_ptr<FTLabel>(new FTLabel(
        header_font, "UX/GL v0.12", startX, startY, framebufferSize.x, framebufferSize.y
    ));
    header->setColor(1.0, 1.0, 1.0, 1.0);
    header->setPixelSize(24);

    startY += 24;

    std::unique_ptr<FTLabel> title = std::unique_ptr<FTLabel>(new FTLabel(
        glfont, "UX/GL INTERFACE v0.12", startX, startY, framebufferSize.x, framebufferSize.y
    ));
    title->setColor(1.0, 1.0, 1.0, 1.0);
    title->setPixelSize(18);

    startY += 16;

    std::unique_ptr<FTLabel> labelMXR = std::unique_ptr<FTLabel>(new FTLabel(
        glfont, "MXR: 0.000000", startX, startY, framebufferSize.x, framebufferSize.y
    ));
    labelMXR->setColor(1.0, 1.0, 1.0, 1.0);
    labelMXR->setPixelSize(18);

    startY += 16;

    std::unique_ptr<FTLabel> labelMYR = std::unique_ptr<FTLabel>(new FTLabel(
        glfont, "MYR: 0.000000", startX, startY, framebufferSize.x, framebufferSize.y
    ));
    labelMYR->setColor(1.0, 1.0, 1.0, 1.0);
    labelMYR->setPixelSize(18);

    startY += 16;

    const std::unique_ptr<FTLabel> labelMZR = std::unique_ptr<FTLabel>(new FTLabel(
        glfont, "MZR: 0.000000", startX, startY, framebufferSize.x, framebufferSize.y
    ));
    labelMZR->setColor(1.0, 1.0, 1.0, 1.0);
    labelMZR->setPixelSize(18);

    startY += 16;

    std::unique_ptr<FTLabel> labelMS = std::unique_ptr<FTLabel>(new FTLabel(
        glfont, "MS/FR: 0.000000", startX, startY, framebufferSize.x, framebufferSize.y
    ));
    labelMS->setColor(1.0, 1.0, 1.0, 1.0);
    labelMS->setPixelSize(18);

    startY += 16;

    std::unique_ptr<FTLabel> labelFPS = std::unique_ptr<FTLabel>(new FTLabel(
        glfont, "FPS: 0.000000", startX, startY, framebufferSize.x, framebufferSize.y
    ));
    labelFPS->setColor(1.0, 1.0, 1.0, 1.0);
    labelFPS->setPixelSize(18);

    startY += 16;

    std::unique_ptr<FTLabel> labelTime = std::unique_ptr<FTLabel>(new FTLabel(
        glfont, "[DATE/TIME]", startX, startY, framebufferSize.x, framebufferSize.y
    ));
    labelTime->setColor(1.0, 1.0, 1.0, 1.0);
    labelTime->setPixelSize(18);



  

    /*
    Homogeneous coordinates
        Until then, we only considered 3D vertices as a(x, y, z) triplet.
        Let’s introduce w.We will now have(x, y, z, w) vectors.

        This will be more clear soon, but for now, just remember this :

    If w == 1, then the vector(x, y, z, 1) is a position in space.
        If w == 0, then the vector(x, y, z, 0) is a direction.
        (In fact, remember this forever.)

        What difference does this make ? Well, for a rotation, it doesn’t change anything.
        When you rotate a point or a direction, you get the same result.
        However, for a translation(when you move the point in a certain direction), things are different.
        What could mean “translate a direction” ? 
        Not much.

        Homogeneous coordinates allow us to use a single mathematical formula to deal with these two cases.
    */

    //std::vector<ux::Ref<ux::Cube>> sub_cubes;
    std::array<ux::Ref<ux::Cube>, 260> sub_cubes;

    size_t sub_cube_count = 20;
    for (size_t n = 0;n < sub_cube_count;n++)
    {
        float angle = ux::PI2 / sub_cube_count * n;
        ux::Ref<ux::Cube> sub_cube1 = ux::CreateRef<ux::Cube>(glm::vec3(1.0));
        glm::mat4 model3 = glm::scale(glm::mat4(1.0), glm::vec3(1.0));
        model3 = rotate(model3, axis, glm::vec3(0.0, angle, 0.0));
        model3 = glm::translate(model3, glm::vec3(10, 0, 0)); // radius   

        
        //glm::mat4 transMat = glm::translate(glm::mat4(1), translate * scale);
        //glm::vec3 newPointVec = transMat * glm::vec4();
        //model3 = trans * rotate(model3, axis, mRotate);
        
        //glm::vec3 new_point = glm::vec3(model3 * glm::vec4(glm::vec4(1.0)));
        //sub_cubes[n].setXYZs(n, new_point);

        sub_cube1->Transformation() = model3;
        sub_cubes[n] = sub_cube1;

    }
    sub_cube_count = 60;
    for (size_t n = 0;n < sub_cube_count;n++)
    {
        float angle = ux::PI2 / sub_cube_count * n;
        ux::Ref<ux::Cube> sub_cube2 = ux::CreateRef<ux::Cube>(glm::vec3(1.0));
        glm::mat4 model4 = glm::scale(glm::mat4(1.0), glm::vec3(1.0));
        //glm::vec3 ss = glm::scale(glm::mat4(1.0), glm::vec3(1.0));
        //glm::vec4 obj = glm::vec4(0);
        //glm::translate();
        //glm::vec3 objTrans = 

        model4 = rotate(model4, axis, glm::vec3(0.0, angle, 0.0));
        model4 = glm::translate(model4, glm::vec3(28, 0, 0)); // radius       

        //glm::mat4 trans = glm::translate(glm::mat4(1), translate * scale);
        //model4 = trans * rotate(model4, axis, mRotate);

        sub_cube2->Transformation() = model4;
        sub_cubes[n + 20] = sub_cube2;
    }
    sub_cube_count = 180;
    for (size_t n = 0;n < sub_cube_count;n++)
    {
        float length = ux::random(8.0);
        //float angle = ux::RADIANS / sub_cube_count * n;
        float angle = ux::random(ux::RADIANS);

        glm::mat4 rm4 = rotate(glm::mat4(1.0), axis, glm::vec3(0.0, angle, 0.0));
                  rm4 = glm::translate(rm4, glm::vec3(42, 0, 0)); // radius

        float radius = 0.1 + ux::random(0.4);
        ux::Ref<ux::Cube> sub_cube2 = ux::CreateRef<ux::Cube>(glm::vec3(0.0, -radius, -radius), glm::vec3(0.5 + length, radius, radius), rm4);

        sub_cube2->Transformation() = glm::mat4(1.0);
        sub_cubes[n + 80] = sub_cube2;
    }


    glm::vec4 amber = ux::COLOR_AMBER;
    amber.a = 0.8;

    float yy = framebufferSize.y - 74;
    auto horzBar = ux::HorzBar(glm::vec2(200.0f, yy - 20), glm::vec2(400.0f, yy));
    horzBar.SetColor(amber);
    horzBar.Build();
   

 
    glm::mat4 modelLight = glm::translate(glm::mat4(1.0), lightPos);

    shaderBase.Bind();
    shaderBase.SetUniformMat4f("u_proj", proj);
    shaderBase.SetUniformMat4f("u_view", view);
    shaderBase.SetUniformMat4f("u_model", modelLight);


    // ORTHO
    glm::mat4 projOrtho = glm::ortho(0.f, (float)framebufferSize.x, 0.0f, (float)framebufferSize.y, 0.1f, zFar); // PROJECTION (SCREEN)
    glm::mat4 viewOrtho = glm::lookAt(glm::vec3(0, 0, 10.0),  // Camera position in world space (doesn't really apply when using an ortho projection matrix)
        glm::vec3(0, 0, 0),  // look at origin
        glm::vec3(0, 1, 0)); // Head is up (set to 0, -1, 0 to look upside down)       
    glm::mat4 modelOrtho = glm::mat4(1.0f); // MODEL
    glm::mat4 mvpOrtho = projOrtho * viewOrtho * modelOrtho;


    std::string soundBackgroundFilename = "res/audio/tlsf/engineloop.wav";

    std::string soundFilename = "res/audio/tlsf/Laser1.wav";

    irrklang::ISoundEngine* engine = irrklang::createIrrKlangDevice();
    
    std::cout << "CAMERA LOCATION: " << glm::to_string(camera_position) << std::endl;
    std::cout << " CAMERA LOOK AT: " << glm::to_string(camera_look_at) << std::endl;

    auto sound_camera_location = irrklang::vec3df(camera_position.x, camera_position.y, camera_position.z);
    auto sound_camera_look_at = irrklang::vec3df(camera_look_at.x, camera_look_at.y, camera_look_at.z);

    engine->setListenerPosition(sound_camera_location, sound_camera_look_at);
    engine->setSoundVolume(1.0f);
    engine->setDefault3DSoundMinDistance(10.0f); // [default] 1.0f

    //engine->play2D(soundBackgroundFilename.c_str(), true); // played looped

    //engine->play2D("res/audio/sound_effect_3.wav");
    auto soundPos = irrklang::vec3df(lightPos.x, lightPos.y, lightPos.z);
    //irrklang::ISound* sound1 = engine->play3D(soundFilename.c_str(), soundPos, true, false, true); // play looped, start paused, track
    //irrklang::ISound* sound1 = engine->play3D(soundBackgroundFilename.c_str(), soundPos, true, false, true); // play looped, start paused, track
    irrklang::ISound* sound1 = NULL;
    //sound1->setMinDistance(5.0f);

    
    //sound1->setPlayPosition(0);
    //sound1->
    //engine->play3D(soundFilename.c_str(), soundPos);


  


    std::string emitterMeshPath = "res/meshes/emitter.obj";
    objl::Loader Loader;
    bool loaded = Loader.LoadFile(emitterMeshPath);

    if (!loaded)
    {
        std::cout << "[ERROR] Failed to load " << emitterMeshPath << std::endl;       
    }
    if (loaded)
    {
        std::cout << "LOADED: " << emitterMeshPath << std::endl;

        for (int i = 0; i < Loader.LoadedMeshes.size(); i++)
        {
            // Copy one of the loaded meshes to be our current mesh
            objl::Mesh curMesh = Loader.LoadedMeshes[i];

            // Print Mesh Name
            std::cout << "Mesh " << i << ": " << curMesh.MeshName << "\n";

            // Print Vertices
            std::cout << "Vertices:\n";

            // Go through each vertex and print its number,
            //  position, normal, and texture coordinate
            for (int j = 0; j < curMesh.Vertices.size(); j++)
            {
                std::cout << "V" << j << ": " <<
                    "P(" << curMesh.Vertices[j].Position.X << ", " << curMesh.Vertices[j].Position.Y << ", " << curMesh.Vertices[j].Position.Z << ") " <<
                    "N(" << curMesh.Vertices[j].Normal.X << ", " << curMesh.Vertices[j].Normal.Y << ", " << curMesh.Vertices[j].Normal.Z << ") " <<
                    "TC(" << curMesh.Vertices[j].TextureCoordinate.X << ", " << curMesh.Vertices[j].TextureCoordinate.Y << ")\n";
            }

            // Print Indices
            std::cout << "Indices:\n";

            // Go through every 3rd index and print the
            //	triangle that these indices represent
            for (int j = 0; j < curMesh.Indices.size(); j += 3)
            {
                std::cout << "T" << j / 3 << ": " << curMesh.Indices[j] << ", " << curMesh.Indices[j + 1] << ", " << curMesh.Indices[j + 2] << "\n";
            }


        }
    }











    glPointSize(5.0);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_LINE_SMOOTH);
    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);
    //glEnable(GL_FRAMEBUFFER_SRGB);
      //glEnable(GL_BLEND);
        //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window))
    {
        if (sound1) {
            auto soundPos = irrklang::vec3df(lightPos.x, lightPos.y, lightPos.z);
            sound1->setPosition(soundPos);
        }


        renderer.Clear();
        
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        //glfwGetTime();

 
        // PERSPECTIVE
        glm::mat4 model = glm::scale(glm::mat4(1.0), glm::vec3(scale)); // MODEL
        model = glm::translate(model, translate);
        bool around_center = true;
        if (around_center)
        {
            // rotates around (0,0,0) of the model.
            model = glm::rotate(model, mRotate[0], glm::vec3(1.0, 0.0, 0.0));
            model = glm::rotate(model, mRotate[1], glm::vec3(0.0, 1.0, 0.0));
            model = glm::rotate(model, mRotate[2], glm::vec3(0.0, 0.0, 1.0));
        }
        else {
            // rotate around the axis.
            model = rotate(model, axis, mRotate);
        }

        glm::mat4 mvp = proj * view * model;

    





        shaderBox.Bind();
        shaderBox.SetUniformMat4f("u_model", model);
        shaderBox.SetUniformMat4f("u_view", view);
        shaderBox.SetUniformMat4f("u_proj", proj);
        for (auto sub_cube : sub_cubes)
        {
            // Performs the transformations after the geometry has been canculated.
            // Keeping the real-time rotation on the screen and stuff in sync.
            glm::mat4 transform = sub_cube->Transformation();
            glm::mat4 trans = glm::translate(glm::mat4(1), translate * scale);
            transform = trans * rotate(transform, axis, mRotate);
            transform = glm::rotate(transform, rotator, glm::vec3(0, 1.0, 0));
            shaderBox.SetUniformMat4f("u_model", transform);
            sub_cube->Draw(renderer, shaderBox);
        }
        shaderBox.SetUniformMat4f("u_model", model);
        segment.Draw(renderer, shaderBox);
        shaderBox.SetUniformMat4f("u_model", glm::rotate(model, (float)(90.0 * ux::TO_RAD), glm::vec3(0.0, 1.0, 0.0)));
        segment.Draw(renderer, shaderBox);
        shaderBox.SetUniformMat4f("u_model", glm::rotate(model, (float)(180.0 * ux::TO_RAD), glm::vec3(0.0, 1.0, 0.0)));
        segment.Draw(renderer, shaderBox);
        shaderBox.SetUniformMat4f("u_model", glm::rotate(model, (float)(270.0 * ux::TO_RAD), glm::vec3(0.0, 1.0, 0.0)));
        segment.Draw(renderer, shaderBox);

        for (float ang = 0.0f;ang < 360.0f;ang += 20.0f)
        {
            shaderBox.SetUniformMat4f("u_model", glm::rotate(model, (float)(ang * ux::TO_RAD), glm::vec3(0.0, 1.0, 0.0)));
            segment2.Draw(renderer, shaderBox);
        }
       


        lightCube.Draw(renderer, shaderBase);
        



        shaderQuad.Bind();
        shaderQuad.SetUniformMat4f("u_MVP", mvp);
        shaderQuad.SetUniform4f("u_Color", 1.0f, 1.0f, 1.0f, 1.0f);
              
        cube.DrawOutline(renderer, shaderQuad);
        ring.Draw(renderer, shaderQuad);


        // border
        shaderQuad.Bind();
        shaderQuad.SetUniformMat4f("u_MVP", mvpOrtho);
        shaderQuad.SetUniform4f("u_Color", 1.0f, 1.0f, 1.0f, 1.0f);
        //horzBar.Draw(renderer, shaderQuad);
        lines.Draw(renderer, shaderQuad);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        shaderUX.Bind();
        shaderUX.SetUniformMat4f("u_MVP", mvpOrtho);
        shaderUX.SetUniform4f("u_Color", 1.0f, 1.0f, 1.0f, 1.0f);
        horzBar.Draw(renderer, shaderUX);
        lines2.Draw(renderer, shaderUX); // why won't it draw if this isn't here?
        



        if (show_text)
        {
            labelMXR->setText2(ux::format("MODEL: % 12.6f % 12.6f % 12.6f", mRotate[0], mRotate[1], mRotate[2]));
            labelMYR->setText2(ux::format("LIGHT: % 12.6f % 12.6f % 12.6f", lightPos[0], lightPos[1], lightPos[2]));
            labelMZR->setText2(ux::format("COLOR: %6.4f %1.4f %1.4f %1.4f", color[0], color[1], color[2], color[3]));
            float msFrame = 1000.0f / ImGui::GetIO().Framerate;
            float frameRate = ImGui::GetIO().Framerate;
            labelMS->setText2(ux::format("MS/FR: %8.6f", msFrame));
            labelFPS->setText2(ux::format("  FPS: %8.3f", frameRate));
            labelTime->setText2(ux::ReadableTime());


            header->render();
            title->render();
            labelMXR->render();
            labelMYR->render();
            labelMZR->render();
            labelMS->render();
            labelFPS->render();
            labelTime->render();
        }
        
        {
            ImGuiColorEditFlags misc_flags = ImGuiColorEditFlags_Float;

            ImGui::Begin("UX/GL");
            //ImGui::SliderFloat4("ABCD", abcd, -3.0f, 3.0f, "%.6f");
            //ImGui::SliderFloat4("EFGH", efgh, -3.0f, 3.0f, "%.6f");
            //ImGui::ColorEdit3("Color", (float*)&color, misc_flags);

            ImGui::SliderFloat3("Model Translate", (float*)&translate, -5.0f, 5.0f);
            ImGui::SliderFloat3("Model Rotate", (float*)&mRotate, 0.0f, ux::PI2);
            ImGui::Separator();
            ImGui::ColorEdit3("Ambient Color", (float*)&ambient_color, misc_flags);
            if (ImGui::IsItemActive()) {
                ubo_Awesome.SetUniform("ux_ambient_color", ambient_color);
            }
            ImGui::ColorEdit3("Diffuse Color", (float*)&diffuse_color, misc_flags);
            ImGui::ColorEdit3("Specular Color", (float*)&specular_color, misc_flags);
            if (ImGui::IsItemActive()) {
                ubo_Awesome.SetUniform("ux_specular_color", specular_color);
            }
            ImGui::SliderFloat("Specular Shininess", &specular_shininess, 0.0f, 100.0f);
            if (ImGui::IsItemActive()) {
                ubo_Awesome.SetUniform("ux_specular_shininess", specular_shininess);
            }
            ImGui::Separator();
            ImGui::SliderFloat3("Camera Position", (float*)&camera_position, -100.0f, 100.0f);
            ImGui::SliderFloat3("Camera Look At", (float*)&camera_look_at, -100.0f, 100.0f);
            ImGui::SliderFloat("Z Near", &zNear, -200.0f, 2000.0f);
            ImGui::SliderFloat("Z Far", &zFar, 0.0f, 2000.0f);
            ImGui::Separator();
            ImGui::SliderFloat3("Light Position", (float*)&lightPos, -100.0f, 100.0f);
            if (ImGui::IsItemActive()) {
                ubo_Awesome.SetUniform("ux_light_pos", glm::vec4(lightPos, 1.0));
                shaderBase.Bind();
                shaderBase.SetUniformMat4f("u_model", glm::translate(glm::mat4(1.0), lightPos));              
            }
            ImGui::ColorEdit3("Light Ambient", (float*)&light_ambient, misc_flags);
            ImGui::ColorEdit3("Light Diffuse", (float*)&light_diffuse, misc_flags);
            ImGui::ColorEdit3("Light Specular", (float*)&light_specular, misc_flags);
            if (ImGui::IsItemActive()) ubo_Awesome.SetUniform("ux_light_specular", light_specular);
            ImGui::SliderFloat3("Light Increment", (float*)&lightInc, -2.0f, 2.0f);
            ImGui::ColorEdit3("s_color1", (float*)&color, misc_flags);
            if (ImGui::IsItemActive()) {
                ubo_Super.SetUniform("s_color1", color);
            }
            ImGui::Separator();
            ImGui::Checkbox("Animate", &animate);
            ImGui::SameLine();
            ImGui::Checkbox("Show Text", &show_text);
            ImGui::SliderFloat("Rotator Increment", &rotator_increment, -2, 2);
            ImGui::SliderFloat("Gamma", &gamma, 0.0f, 5.0f);
            if (ImGui::IsItemActive()) ubo_Awesome.SetUniform("ux_gamma", gamma);
            if (ImGui::Button("Play Sound"))
            {
                auto soundPos = irrklang::vec3df(lightPos.x, lightPos.y, lightPos.z);
                engine->play3D(soundFilename.c_str(), soundPos);
            }
            if (ImGui::Button("Exit"))
            {
                std::cout << "EXIT" << std::endl;
                break;
            }
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();

            ImGui::ShowDemoWindow();
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        
        if (animate)
        {
            lightPos += lightInc;

            if (lightPos.x > 65.0f || lightPos.x < -65.0f)
            {
                lightInc.x = -lightInc.x;
            }
            ubo_Awesome.SetUniform("ux_light_pos", glm::vec4(lightPos, 1.0));
            shaderBase.Bind();
            shaderBase.SetUniformMat4f("u_model", glm::translate(glm::mat4(1.0), lightPos));
            /*
            mRotate += (0.00025, 0.00025, 0.00025);
            if (mRotate.x >= ux::PI2)
            {
                mRotate = glm::vec3(0.0);
            }
            */
            if (rotator_increment > 0.0)
            {
                rotator += rotator_increment;
                if (0.0f > rotator > ux::RADIANS)
                {
                    rotator_increment *= -1;
                }
            }
        }
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    // cleanup audio
    if (sound1) sound1->drop(); // release music stream.
    engine->drop();

    // Cleanup  
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    
    glfwDestroyWindow(window);
    glfwTerminate();
    
    return 0;
}