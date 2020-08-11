#include <glad/glad.h>  
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <chrono>
#include <thread>
#include <ctime>

//#define SPDLOG_HEADER_ONLY
//#define SPDLOG_COMPILED_LIB
//#include "spdlog/spdlog.h";
// some magic needed to get spdlog working!

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "lux/Window.hpp"
#include "lux/Cube.h"
#include "lux/Segment.hpp"
#include "lux/Lines.hpp"
#include "lux/TextList.hpp"
#include "lux/Cuboid.hpp"
#include "lux/Color.hpp"
#include "lux/HorzBar.hpp"
#include "lux/Interface/ScoopedCorner.hpp"
#include "lux/Ring.hpp"
#include "Texture.h"
#include "lux/Scene/Camera.hpp"
#include "lux/Renderer/Canvas.hpp"
#include "lux/Renderer/Skybox.hpp"
#include "lux/Renderer/UniformBuffer.hpp"


#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
//#define IMAPP_IMPL
//#include "imgui/ImApp.h"
//#include "imgui/ImGuizmo.h"



#include "entt/entt.hpp"

#include <irrKlang/irrKlang.h>

#include "nlohmann/json.hpp"

//#define FMT_HEADER_ONLY
//#include "fmt/format.h"


// If w == 0, the value stored is a vector, if w == 1, the value stored is a point.

// swizzling means rearranging the elements of a vector (kinda).

// Transform Feedback Buffer
// http://ogldev.atspace.co.uk/www/tutorial28/tutorial28.html
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

// entt::entity e_Handle { entt::null }; // saving for future reference

namespace lux {

    inline static glm::mat4 rotateAroundAxis(glm::mat4 model, glm::vec3 axis, glm::vec3 angle)
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

    glm::mat4 transform(glm::mat4 root, glm::vec3 scale, glm::vec3 translate, glm::vec3 rotate)
    {
        glm::mat4 m2scale = glm::scale(glm::mat4(1.0), scale);
        glm::mat4 m2trans = glm::translate(glm::mat4(1.0), translate);
        glm::mat4 m2rotate1 = glm::rotate(glm::mat4(1.0), rotate[0], glm::vec3(1.0f, 0.0f, 0.0f));
        glm::mat4 m2rotate2 = glm::rotate(glm::mat4(1.0), rotate[1], glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 m2rotate3 = glm::rotate(glm::mat4(1.0), rotate[2], glm::vec3(0.0f, 0.0f, 1.0f));
        glm::mat4 matrix = m2rotate3 * m2rotate2 * m2rotate1 * m2trans * m2scale;
        return root * matrix;
    }

    std::string to_uppercase(std::string text)
    {
        std::transform(text.begin(), text.end(), text.begin(),
            [](unsigned char c) { return std::toupper(c); });
        return text;
    }
};


int main(int argc, char** argv)
{
    bool animate = true;
    bool show_text = false;
    bool use_imgui = true;
    bool show_models = true;


    auto window = lux::Window("LUX/GL", 1500, 900);
    window.Center();


    


    auto cube = lux::Cube(glm::vec3(1.0f));

    auto segment = lux::Segment(0.5f, 1.0f, 0.0f, 45.0f, 0.125f);

    auto segment2 = lux::Segment(1.25f, 1.5f, 0.0f, 15.0f, 0.125f);


 

    

    //auto texture = Texture("res/textures/cartographer-logo-600x600.png");
    //texture.Bind();
    //shader.SetUniform1i("u_Texture", 0);

    auto shader = lux::Shader("res/shaders/basic.shader");
    auto shaderQuad = lux::Shader("res/shaders/quad.shader");
    auto shaderBox = lux::Shader("res/shaders/box.shader");
    auto shaderBase = lux::Shader("res/shaders/base.shader");
    auto shaderBase2 = lux::Shader("res/shaders/base.shader");
    auto shaderText = lux::Shader("res/shaders/text.shader");
    auto shaderUX = lux::Shader("res/shaders/ux.shader");

  


    glm::vec3 axis = { 0.0, 0.0, 0.0 };

    // all object in the scene are scaled by scale
    float scale = 3.0f; // 5.0 1.0; //0.125;
    //glm::vec3 translate = { -1.4, 0.0, 0.0 };
    //glm::vec3 mRotate = { 0.5, 0.0, 2.0 };
    glm::vec3 translate = { -20.0, 0.299, 0.0 };
    glm::vec3 mRotate = { 0.0, 0.0, 0.0 };
    float translateZ = -100.0;
    glm::vec2 rotateXY = glm::vec2(0.0, glm::radians(0.0));

    float scale2 = 1.0f;
    glm::vec3 translate2 = { 0.0f, 0.0f, 0.0f };
    glm::vec3 rotate2 = { 0.938f, 0.469f, 0.0f };

    float rotator = 0.0;
    float rotator_increment = 0.0; //0.001;

    glm::vec4 color = { 0.2, 0.2, 0.2, 1.0 };


    // lighting
    //glm::vec3 lightPos = glm::vec3(-24.0, 1.0, 1.0);
    glm::vec3 lightPos = glm::vec3(13.0, 17.0, 1.0);
    glm::vec3 lightInc = glm::vec3(0.045, 0.0, 0.0);

    glm::vec4 light_color = { 1.0, 1.0, 1.0, 1.0 };
    glm::vec4 light_ambient = { 0.2, 0.2, 0.2, 1.0 };
    glm::vec4 light_diffuse = { 0.5, 0.5, 0.5, 1.0 };
    glm::vec4 light_specular = { 0.2, 0.2, 0.2, 1.0 };


    lux::Light light1 = {
        lightPos,
        light_ambient,
        light_diffuse,
        light_specular
    };

    lux::Light light2 = {
        glm::vec3(1.0f, 1.0f, -24.0f),
        light_ambient,
        light_diffuse,
        light_specular
    };

    lux::Light lights[2] = { 
        light1, 
        light2 
    };

    //std::cout << "Lights Size: " << sizeof(lights) << std::endl;

    auto ubo_Lights = lux::UniformBuffer("LightProperties", 3, 128, &lights);
    ubo_Lights.AddUniform("lights[0].position", 0, 16);
    ubo_Lights.AddUniform("lights[0].ambient_color", 16, 12);
    ubo_Lights.AddUniform("lights[0].diffuse_color", 32, 12);
    ubo_Lights.AddUniform("lights[0].specular_color", 48, 12);

    ubo_Lights.AddUniform("lights[1].position", 64, 16);
    ubo_Lights.AddUniform("lights[1].ambient_color", 80, 12);
    ubo_Lights.AddUniform("lights[1].diffuse_color", 96, 12);
    ubo_Lights.AddUniform("lights[1].specular_color", 112, 12);


    ubo_Lights.SetData(lights);


   
    uint32_t ACTIVE_CAMERA = 0;

  
  

    lux::Material material = {
        { 0.348f, 0.348f, 0.348f, 1.0f },    // ambient color
        { 0.608f, 0.608f, 0.608f, 1.0f },    // diffuse color
        { 0.5f, 0.5f, 0.5f, 1.0f },          // specular color
        32.0f                                // specular shininess
    };

    

    auto ubo_Materials = lux::UniformBuffer("MaterialProperties", 4, 64, &material);
    ubo_Materials.AddUniform("material.ambient_color", 0, 12);
    ubo_Materials.AddUniform("material.diffuse_color", 16, 12);
    ubo_Materials.AddUniform("material.specular_color", 32, 12);
    ubo_Materials.AddUniform("material.specular_shininess", 48, 4);

  


    lux::Renderer renderer;

    lux::Camera camera = lux::Camera(glm::vec3(0.0f,0.0f,50.0f), glm::vec3(0.0f,0.0f,0.0f));
    camera.SetAspectRatio(window.GetAspectRatio());
    glm::mat4 view = camera.GetView();
    glm::mat4 proj = camera.GetProjection();



    lux::SceneProperties scenes[1] = {

           glm::mat4(1.0f),
           view,
           proj,
           renderer.Gamma()
    };


    auto ubo_SceneProperties = lux::UniformBuffer("SceneProperties", 6, 208, &scenes);
    ubo_SceneProperties.AddUniform("scenes[0].model", 0, 64);
    ubo_SceneProperties.AddUniform("scenes[0].view", 64, 64);
    ubo_SceneProperties.AddUniform("scenes[0].proj", 128, 64);
    ubo_SceneProperties.AddUniform("scenes[0].gamma", 192, 4);




    



    auto lightCube = lux::Cube(glm::vec3(1.0f, 1.0f, 1.0f));
    auto lightCube2 = lux::Cube(glm::vec3(1.0f, 1.0f, 1.0f));



    auto lines = lux::Lines();
    //lines.add(glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    //lines.add(glm::vec3(0.5, 0, 0), glm::vec3(0.5, 1, 0));
    lines.add(glm::vec3(0.0f, window.GetFramebufferWidth() - 1, 0.0f), glm::vec3(window.GetFramebufferWidth() - 1, window.GetFramebufferHeight() - 1, 0.0f)); // TOP
    lines.add(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(window.GetFramebufferWidth() - 1, 0.0f, 0.0f)); // BOTTOM
    lines.add(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, window.GetFramebufferHeight() - 1, 0.0f)); // LEFT
    lines.add(glm::vec3(window.GetFramebufferWidth() - 1, 0.0f, 0.0f), glm::vec3(window.GetFramebufferWidth() - 1, window.GetFramebufferHeight() - 1, 0.0f)); // RIGHT
    lines.commit();

    auto lines2 = lux::Lines();
    lines2.createCircle(2.0, 360);

    auto ring = lux::Lines();
    ring.createRing(2.2, 2.3, 0.1, 180);

    auto triangles = lux::Ring(2.2, 2.3, 0.1, 180);
    triangles.Build();



 
    auto textList = lux::TextList(50, 50, window.GetFramebufferWidth(), window.GetFramebufferHeight());
    textList.AddFont(0, "res/fonts/Inconsolata/static/InconsolataCondensed-Medium.ttf");
    textList.AddFont(1, "res/fonts/Inconsolata/static/InconsolataCondensed-Light.ttf");
    textList.AddText(0, 0, 24, 24, "LUX/GL v0.12a");
    textList.AddText(1, 1, 18, 18, "LCARS USER EXPERIENCE GRAPHICS LIBRARY");
    textList.AddText(1, 2, 18, 18, "MXR: 0.000000");
    textList.AddText(1, 3, 18, 18, "MYR: 0.000000");
    textList.AddText(1, 4, 18, 18, "MZR: 0.000000");
    textList.AddText(1, 5, 18, 18, "MS/FR: 0.000000");
    textList.AddText(1, 6, 18, 18, "FPS: 0.000000");
    textList.AddText(1, 7, 18, 18, "[DATE/TIME]");




    //std::vector<ux::Ref<ux::Cube>> sub_cubes;
    std::array<lux::Ref<lux::Cube>, 260> sub_cubes;

    size_t sub_cube_count = 20;
    for (size_t n = 0;n < sub_cube_count;n++)
    {
        float angle = lux::PI2 / sub_cube_count * n;
        lux::Ref<lux::Cube> sub_cube1 = lux::CreateRef<lux::Cube>(glm::vec3(1.0));
        glm::mat4 model3 = glm::scale(glm::mat4(1.0), glm::vec3(1.0));
        model3 = lux::rotateAroundAxis(model3, axis, glm::vec3(0.0, angle, 0.0));
        model3 = glm::translate(model3, glm::vec3(10, 0, 0)); // radius   

        sub_cube1->Transformation() = model3;
        sub_cubes[n] = sub_cube1;

    }
    sub_cube_count = 60;
    for (size_t n = 0;n < sub_cube_count;n++)
    {
        float angle = lux::PI2 / sub_cube_count * n;
        lux::Ref<lux::Cube> sub_cube2 = lux::CreateRef<lux::Cube>(glm::vec3(1.0));
        glm::mat4 model4 = glm::scale(glm::mat4(1.0), glm::vec3(0.25));

        model4 = lux::rotateAroundAxis(model4, axis, glm::vec3(0.0, angle, 0.0));
        model4 = glm::translate(model4, glm::vec3(11.5f, 0, 0)); // radius       

        sub_cube2->Transformation() = model4;
        sub_cubes[n + 20] = sub_cube2;
    }
    sub_cube_count = 180;
    for (size_t n = 0;n < sub_cube_count;n++)
    {
        float length = lux::random(8.0);
        float angle = lux::random(lux::RADIANS);

        glm::mat4 rm4 = lux::rotateAroundAxis(glm::mat4(1.0), axis, glm::vec3(0.0, angle, 0.0));
                  rm4 = glm::translate(rm4, glm::vec3(4, 0, 0)); // radius

        float radius = 0.05 + lux::random(0.1);
        lux::Ref<lux::Cube> sub_cube2 = lux::CreateRef<lux::Cube>(glm::vec3(0.0, -radius, -radius), glm::vec3(0.5 + length, radius, radius), rm4);

        sub_cube2->Transformation() = glm::mat4(1.0);
        sub_cubes[n + 80] = sub_cube2;
    }


    
    const int moreCount = 300;
    std::array<lux::Ref<lux::Cuboid>, moreCount> moreCubes;
    for (size_t n = 0;n < moreCount;n++)
    {
        float length = lux::random(5.0);
        float angle = lux::random(lux::RADIANS);

        glm::mat4 rm4 = lux::rotateAroundAxis(glm::mat4(1.0), axis, glm::vec3(0.0, angle, 0.0));
        rm4 = rm4 * glm::translate(rm4, glm::vec3(8, 0, 0)); // radius

        float radius = 0.025 + lux::random(0.1);
        lux::Ref<lux::Cuboid> moreCube = lux::CreateRef<lux::Cuboid>(glm::vec3(0.0, -radius, -radius), glm::vec3(0.5 + length, radius, radius), glm::mat4(1.0f));
        moreCube->Build();

        moreCube->SetTransformation(rm4);
        moreCubes[n] = moreCube;
    }

    


    auto cuboid = lux::Cuboid(glm::vec3(-0.2f), glm::vec3(0.2f), glm::mat4(1.0f));
    cuboid.Build();




    glm::vec4 amber = lux::COLOR_AMBER;
    amber.a = 0.8;

    float yy = window.GetFramebufferHeight() - 152;
    auto horzBar = lux::HorzBar(glm::vec2(164.0f, yy - 20), glm::vec2(400.0f, yy));
    horzBar.SetColor(amber);
    horzBar.Build();
   

 
    glm::mat4 modelLight = glm::translate(glm::mat4(1.0), lightPos);

    shaderBase.Bind();
    shaderBase.SetUniformMat4f("u_proj", proj);
    shaderBase.SetUniformMat4f("u_view", view);
    shaderBase.SetUniformMat4f("u_model", modelLight);

    shaderBase2.Bind();
    shaderBase2.SetUniformMat4f("u_proj", proj);
    shaderBase2.SetUniformMat4f("u_view", view);
    shaderBase2.SetUniformMat4f("u_model", modelLight);


    // ORTHO
    
    glm::mat4 projOrtho = glm::ortho(0.f, (float)window.GetFramebufferWidth(), 0.0f, (float)window.GetFramebufferHeight(), 0.01f, 2000.0f); // PROJECTION (SCREEN)
    glm::mat4 viewOrtho = glm::lookAt(glm::vec3(0, 0, 10.0),  // Camera position in world space (doesn't really apply when using an ortho projection matrix)
        glm::vec3(0, 0, 0),  // look at origin
        glm::vec3(0, 1, 0)); // Head is up (set to 0, -1, 0 to look upside down)       
    glm::mat4 modelOrtho = glm::mat4(1.0f); // MODEL
    glm::mat4 mvpOrtho = projOrtho * viewOrtho * modelOrtho;
    

    std::string soundBackgroundFilename = "res/audio/tlsf/engineloop.wav";

    std::string soundFilename = "res/audio/tlsf/Laser1.wav";

    irrklang::ISoundEngine* engine = irrklang::createIrrKlangDevice();
    
    //std::cout << "CAMERA LOCATION: " << glm::to_string(camera_position) << std::endl;
    //std::cout << " CAMERA LOOK AT: " << glm::to_string(camera_look_at) << std::endl;

    auto sound_camera_location = irrklang::vec3df(camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
    auto sound_camera_look_at = irrklang::vec3df(camera.GetlookAt().x, camera.GetlookAt().y, camera.GetlookAt().z);

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





    
    lux::Mesh emitterMesh = lux::Mesh();
    emitterMesh.Load("Cube_Cube.003", "res/meshes/emitter.obj");
    //emitterMesh.Load("head_Cube", "res/meshes/Toon-Female-blockout.obj");
    emitterMesh.Build();
    



    /*
    entt::registry registry;

    auto entity = registry.create();

    registry.emplace<glm::vec3>(entity, 10.0f, 10.0f, 10.0f);

    glm::vec3 out = registry.get<glm::vec3>(entity);

    std::cout << "OUT: " << glm::to_string(out) << std::endl;

    registry.destroy(entity);
    */

 


    lux::Canvas canvas = lux::Canvas();
    canvas.Init(window.GetWidth(), window.GetHeight());


    auto scoopedCorner = lux::ScoopedCorner(glm::vec2(10.0f, 10.0f), 50.0f, amber);
    scoopedCorner.Build();


    if (use_imgui)
    {
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        const char* glsl_version = "#version 450";
        ImGui::CreateContext();
        //ImGuiIO& io = ImGui::GetIO(); (void)io;
        // Setup Platform/Renderer bindings
        ImGui_ImplGlfw_InitForOpenGL(window.GetNativeWindow(), true);
        ImGui_ImplOpenGL3_Init(glsl_version);
        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsClassic();
    }

    auto skybox = lux::Skybox();
    skybox.Init();

    auto cubemapShader = lux::Shader("res/shaders/cubemap-shader.glsl");
    //auto cubemapShader = Shader("res/shaders/cubemap-lighting-shader.glsl");

    //auto testShader = cubemapShader;
    auto testShader = shaderBox;



    while (window.Loop())
    {
        if (sound1) {
            auto soundPos = irrklang::vec3df(lightPos.x, lightPos.y, lightPos.z);
            sound1->setPosition(soundPos);
        }
   
        canvas.Begin();
        renderer.Clear();      
        
        // PERSPECTIVE
        glm::mat4 model = lux::transform(glm::mat4(1.0f), glm::vec3(scale, scale, scale), translate, mRotate);
        model = lux::transform(model, glm::vec3(scale2, scale2, scale2), translate2, rotate2);
        glm::mat4 mvp = proj * view * model;

        testShader.SetUniformMat4f("u_view", view);
        testShader.SetUniformMat4f("u_proj", proj);       
        testShader.SetUniformMat4f("u_model", model);
        testShader.SetUniform1f("u_time", renderer.GetTime());
        //cubemapShader.SetUniformMat4f("view", view);
        //cubemapShader.SetUniformMat4f("projection", proj);
        //cubemapShader.SetUniformMat4f("model", model * sub_cube->Transformation());
 
        //cuboid.Draw(renderer, shaderBox);

        for (auto sub_cube : sub_cubes)
        {
            testShader.SetUniformMat4f("u_model", model * sub_cube->Transformation());
            sub_cube->Draw(renderer, testShader); 
        }
        
        for (auto moreCube : moreCubes)
        {
            testShader.SetUniformMat4f("u_model", model * moreCube->GetTransformation());
            moreCube->Draw(renderer, testShader);
        }
        
        testShader.SetUniformMat4f("u_model", model);
        emitterMesh.Draw(renderer, testShader);
        
        //testShader.SetUniformMat4f("u_model", model);
        segment.Draw(renderer, testShader);
        testShader.SetUniformMat4f("u_model", glm::rotate(model, (float)(90.0 * lux::TO_RAD), glm::vec3(0.0, 1.0, 0.0)));
        segment.Draw(renderer, testShader);
        testShader.SetUniformMat4f("u_model", glm::rotate(model, (float)(180.0 * lux::TO_RAD), glm::vec3(0.0, 1.0, 0.0)));
        segment.Draw(renderer, testShader);
        testShader.SetUniformMat4f("u_model", glm::rotate(model, (float)(270.0 * lux::TO_RAD), glm::vec3(0.0, 1.0, 0.0)));
        segment.Draw(renderer, testShader);

        for (float ang = 0.0f;ang < 360.0f;ang += 20.0f)
        {
            testShader.SetUniformMat4f("u_model", glm::rotate(model, (float)(ang * lux::TO_RAD), glm::vec3(0.0, 1.0, 0.0)));
            segment2.Draw(renderer, testShader);
        }
        
        


        // note this used shaderBase!!!
        lightCube.Draw(renderer, shaderBase);
        lightCube2.Draw(renderer, shaderBase2);
        



        shaderQuad.Bind();
        shaderQuad.SetUniformMat4f("u_MVP", mvp);
        shaderQuad.SetUniform4f("u_Color", 1.0f, 1.0f, 1.0f, 1.0f);           
        // wireframe cube
        cube.DrawOutline(renderer, shaderQuad);
        ring.Draw(renderer, shaderQuad);

        // border
        shaderQuad.SetUniformMat4f("u_MVP", mvpOrtho);
        shaderQuad.SetUniform4f("u_Color", 1.0f, 1.0f, 1.0f, 1.0f);
        lines.Draw(renderer, shaderQuad);
        

        shaderUX.SetUniformMat4f("u_MVP", mvpOrtho);
        shaderUX.SetUniform4f("u_Color", 1.0f, 1.0f, 1.0f, 1.0f);    
        scoopedCorner.Draw(renderer, shaderUX);
        horzBar.Draw(renderer, shaderUX);
        

        
        //skybox.Render(camera_position, proj);
        

        if (show_text)
        {
            // TODO text can be made faster here!
            textList.SetText(2, lux::format("MODEL: % 12.6f % 12.6f % 12.6f", mRotate[0], mRotate[1], mRotate[2]));
            textList.SetText(3, lux::format("LIGHT: % 12.6f % 12.6f % 12.6f", lightPos[0], lightPos[1], lightPos[2]));
            textList.SetText(4, lux::format("COLOR: %6.4f %1.4f %1.4f %1.4f", color[0], color[1], color[2], color[3]));
            float msFrame = 1000.0f / ImGui::GetIO().Framerate;
            float frameRate = ImGui::GetIO().Framerate;
            textList.SetText(5, lux::format("MS/FR: %8.6f", msFrame));
            textList.SetText(6, lux::format("  FPS: %8.3f", frameRate));
            textList.SetText(7, lux::to_uppercase(lux::ReadableTime()));

            textList.Draw();
        }
        
        if (use_imgui)
        {
            // Start the Dear ImGui frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            {
                ImGuiColorEditFlags misc_flags = ImGuiColorEditFlags_Float;

                ImGui::Begin("UX/SIM/GL");

                //ImGui::SliderFloat4("ABCD", abcd, -3.0f, 3.0f, "%.6f");
                //ImGui::SliderFloat4("EFGH", efgh, -3.0f, 3.0f, "%.6f");
                //ImGui::ColorEdit3("Color", (float*)&color, misc_flags);
                ImGui::SliderFloat("M1 Scale", &scale, 1.0f, 50.0f);
                ImGui::SliderFloat3("M1 Translate", (float*)&translate, -20.0f, 20.0f);
                ImGui::SliderFloat3("M1 Rotate", (float*)&mRotate, 0.0f, lux::PI2);
                ImGui::Separator();
                ImGui::SliderFloat("M2 Scale", &scale2, 1.0f, 50.0f);
                ImGui::SliderFloat3("M2 Translate", (float*)&translate2, -20.0f, 20.0f);
                ImGui::SliderFloat3("M2 Rotate", (float*)&rotate2, 0.0f, lux::PI2);
                ImGui::Separator();
                // MATERIAL
                ImGui::ColorEdit3("Ambient Color", (float*)&material.ambient_color, misc_flags);
                if (ImGui::IsItemActive()) ubo_Materials.SetData(&material); //ubo_Materials.SetUniformVec3("material.ambient_color", material.ambient_color);
                ImGui::ColorEdit3("Diffuse Color", (float*)&material.diffuse_color, misc_flags);
                if (ImGui::IsItemActive()) ubo_Materials.SetData(&material); //ubo_Materials.SetUniformVec3("material.diffuse_color", material.diffuse_color);
                ImGui::ColorEdit3("Specular Color", (float*)&material.specular_color, misc_flags);
                if (ImGui::IsItemActive()) ubo_Materials.SetData(&material); //ubo_Materials.SetUniformVec3("material.specular_color", material.specular_color);
                ImGui::SliderFloat("Specular Shininess", &material.specular_shininess, 0.0f, 100.0f);
                if (ImGui::IsItemActive()) ubo_Materials.SetData(&material); //ubo_Materials.SetUniform1f("material.specular_shininess", material.specular_shininess);
                ImGui::Separator();

                //ImGui::SliderFloat3("Camera Position", (float*)&camera.Position(), -100.0f, 100.0f);
                //ImGui::SliderFloat3("Camera Look At", (float*)&camera_look_at, -100.0f, 100.0f);
                //ImGui::SliderFloat("Z Near", &zNear, -200.0f, 2000.0f);
                //ImGui::SliderFloat("Z Far", &zFar, 0.0f, 2000.0f);
                ImGui::Separator();
                ImGui::SliderFloat3("Light Position", (float*)&lightPos, -100.0f, 100.0f);
                if (ImGui::IsItemActive()) {
                    ubo_Lights.SetUniformVec4("lights[0].position", glm::vec4(lightPos, 1.0f));
                    //ubo_Awesome.SetUniform("ux_light_pos", glm::vec4(lightPos, 1.0f));
                    shaderBase.Bind();
                    shaderBase.SetUniformMat4f("u_model", glm::translate(glm::mat4(1.0f), lightPos));
                }
                ImGui::ColorEdit3("Light Ambient", (float*)&light_ambient, misc_flags);
                if (ImGui::IsItemActive()) ubo_Lights.SetUniformVec3("lights[0].ambient_color", light_ambient);

                ImGui::ColorEdit3("Light Diffuse", (float*)&light_diffuse, misc_flags);
                if (ImGui::IsItemActive()) ubo_Lights.SetUniformVec3("lights[0].diffuse_color", light_diffuse);
                ImGui::ColorEdit3("Light Specular", (float*)&light_specular, misc_flags);
                if (ImGui::IsItemActive()) ubo_Lights.SetUniformVec3("lights[0].specular_color", light_specular);
                ImGui::SliderFloat3("Light Increment", (float*)&lightInc, -2.0f, 2.0f);

                ImGui::ColorEdit3("s_color1", (float*)&color, misc_flags);
                //if (ImGui::IsItemActive()) ubo_Super.SetUniform("s_color1", color);
                ImGui::Separator();
                ImGui::Checkbox("Animate", &animate);
                ImGui::SameLine();
                ImGui::Checkbox("Show Text", &show_text);
                ImGui::SliderFloat("Rotator Increment", &rotator_increment, -2, 2);
                ImGui::SliderFloat("Gamma", &renderer.Gamma(), 0.0f, 5.0f);
                if (ImGui::IsItemActive()) ubo_SceneProperties.SetUniform1f("scenes[0].gamma", renderer.Gamma());
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
                /*
                int m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
                float m_SnapValue = 0.5f;
                float rw = (float)ImGui::GetWindowWidth();
                float rh = (float)ImGui::GetWindowHeight();
                ImGuizmo::SetOrthographic(false);
                ImGuizmo::SetDrawlist();
                ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, rw, rh);
                bool snap = false; //Input::IsKeyPressed(HZ_KEY_LEFT_CONTROL);
                
                ImGuizmo::DrawGrid(&view[0][0], &proj[0][0], &model[0][0], 100.f);
                ImGuizmo::DrawCubes(&view[0][0], &proj[0][0], &model[0][0], 1);
                ImGuizmo::Manipulate(glm::value_ptr(view * model),
                    glm::value_ptr(proj),
                    (ImGuizmo::OPERATION)m_GizmoType,
                    ImGuizmo::LOCAL,
                    glm::value_ptr(model),
                    nullptr,
                    snap ? &m_SnapValue : nullptr);
                */
                /*
                ImGuizmo::Manipulate(glm::value_ptr(m_ActiveScene->GetCamera().GetViewMatrix()* m_MeshEntity->Transform()),
                    glm::value_ptr(m_ActiveScene->GetCamera().GetProjectionMatrix()),
                    (ImGuizmo::OPERATION)m_GizmoType,
                    ImGuizmo::LOCAL,
                    glm::value_ptr(*m_CurrentlySelectedTransform),
                    nullptr,
                    snap ? &m_SnapValue : nullptr);
                    */
                    //ImGuizmo::DrawGrid(cameraView, cameraProjection, identityMatrix, 100.f);
                    //ImGuizmo::DrawCubes(cameraView, cameraProjection, &objectMatrix[0][0], gizmoCount);
                ImGui::End();

                ImGui::ShowDemoWindow();
            }

            ImGui::Render();
            ImGui::EndFrame();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }
        
        if (animate)
        {
            lightPos += lightInc;

            if (lightPos.x > 65.0f || lightPos.x < -65.0f)
            {
                lightInc.x = -lightInc.x;
            }
            //ubo_Awesome.SetUniform("ux_light_pos", glm::vec4(lightPos, 1.0));
            ubo_Lights.SetUniformVec4("lights[0].position", glm::vec4(lightPos, 1.0));
            shaderBase.Bind();
            shaderBase.SetUniformMat4f("u_model", glm::translate(glm::mat4(1.0), lightPos));

            glm::vec3 lp2 = glm::vec3(1.0f, lightPos.y, lightPos.x);
            //glm::mat4 lp2m = lux::rotateAroundAxis(glm::mat4(1.0f), axis, mRotate);

            ubo_Lights.SetUniformVec4("lights[1].position", glm::vec4(lp2, 1.0f));
            shaderBase2.Bind();
            shaderBase2.SetUniformMat4f("u_model", glm::translate(glm::mat4(1.0), lp2));
            //shaderBase2.SetUniformMat4f("u_model", lp2m);
            /*
            mRotate += (0.00025, 0.00025, 0.00025);
            if (mRotate.x >= lux::PI2)
            {
                mRotate = glm::vec3(0.0);
            }
            */
            if (rotator_increment > 0.0)
            {
                rotator += rotator_increment;
                if (0.0f > rotator > lux::RADIANS)
                {
                    rotator_increment *= -1;
                }
            }
        }
     
        canvas.End();
        canvas.Render();
      
        window.ContinueLoop();
        //glfwSwapBuffers(window);
        //glfwPollEvents();
    }
    
    // cleanup audio
    if (sound1) sound1->drop(); // release music stream.
    engine->drop();

    if (use_imgui)
    {
        // Cleanup  
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    window.Close();   
    return 0;
}