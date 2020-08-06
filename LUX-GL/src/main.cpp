﻿#include <GL/glew.h>                       // Cross platform link between your graphics card and OpenGL.
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

#include "ux/Window.hpp"
#include "ux/Cube.h"
#include "ux/Segment.hpp"
#include "ux/Lines.hpp"
#include "ux/TextList.hpp"
#include "ux/Cuboid.hpp"
#include "ux/Color.hpp"
#include "ux/HorzBar.hpp"
#include "ux/primitive/ScoopedCorner.hpp"
#include "ux/Ring.hpp"
#include "Texture.h"

#include "UniformBuffer.hpp"
#include "FrameBuffer.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "entt/entt.hpp"

#include <irrKlang/irrKlang.h>

#include "nlohmann/json.hpp"

//#define FMT_HEADER_ONLY
//#include "fmt/format.h"


// If w == 0, the value stored is a vector, if w == 1, the value stored is a point.

// swizzling means rearranging the elements of a vector.

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

namespace ux {

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

};


int main(int argc, char** argv)
{
    bool animate = false;
    bool show_text = true;
    bool use_render_buffer = true;
    bool use_imgui = true;
    bool show_models = true;


    auto window = ux::Window("LUX/GL", 1500, 900);
    window.Center();


    std::cout << "[WINDOW SIZE] " << window.GetWidth() << " " << window.GetHeight() << std::endl;


    auto cube = ux::Cube(glm::vec3(1.0f));

    auto segment = ux::Segment(0.5f, 1.0f, 0.0f, 45.0f, 0.125f);

    auto segment2 = ux::Segment(1.25f, 1.5f, 0.0f, 15.0f, 0.125f);


 

    auto shader = Shader("res/shaders/basic.shader");

    //auto texture = Texture("res/textures/cartographer-logo-600x600.png");
    //texture.Bind();
    //shader.SetUniform1i("u_Texture", 0);

    auto shaderQuad = Shader("res/shaders/quad.shader");
    auto shaderBox = Shader("res/shaders/box.shader");
    auto shaderBase = Shader("res/shaders/base.shader");
    auto shaderBase2 = Shader("res/shaders/base.shader");
    auto shaderText = Shader("res/shaders/text.shader");
    auto shaderUX = Shader("res/shaders/ux.shader");

  



    glm::vec3 axis = { 0.0, 0.0, 0.0 };

    // all object in the scene are scaled by scale
    float scale = 5.0; //1.0; //0.125;
    //glm::vec3 translate = { -1.4, 0.0, 0.0 };
    //glm::vec3 mRotate = { 0.5, 0.0, 2.0 };
    glm::vec3 translate = { 0.0, 0.0, 0.0 };
    glm::vec3 mRotate = { 0.0, 0.0, 0.0 };


    float scale2 = 1.0f;
    glm::vec3 translate2 = { 0.0f, 0.0f, 0.0f };
    glm::vec3 rotate2 = { 0.0f, 0.0f, 0.0f };



    float rotator = 0.0;
    float rotator_increment = 0.0; //0.001;




    float translateZ = -100.0;
    glm::vec2 rotateXY = glm::vec2(0.0, glm::radians(0.0));


    glm::vec4 color = { 0.2, 0.2, 0.2, 1.0 };


    // lighting
    //glm::vec3 lightPos = glm::vec3(-24.0, 1.0, 1.0);
    glm::vec3 lightPos = glm::vec3(13.0, 17.0, 38.0);
    glm::vec3 lightInc = glm::vec3(0.045, 0.0, 0.0);

    glm::vec4 light_color = { 1.0, 1.0, 1.0, 1.0 };
    glm::vec4 light_ambient = { 0.2, 0.2, 0.2, 1.0 };
    glm::vec4 light_diffuse = { 0.5, 0.5, 0.5, 1.0 };
    glm::vec4 light_specular = { 0.2, 0.2, 0.2, 1.0 };


    ux::Light light1 = {
        lightPos,
        light_ambient,
        light_diffuse,
        light_specular
    };

    ux::Light light2 = {
        glm::vec3(1.0f, 1.0f, -24.0f),
        light_ambient,
        light_diffuse,
        light_specular
    };

    ux::Light lights[2] = { 
        light1, 
        light2 
    };

    std::cout << "Lights Size: " << sizeof(lights) << std::endl;

    auto ubo_Lights = UniformBuffer("LightProperties", 3, 128, &lights);
    ubo_Lights.AddUniform("lights[0].position", 0, 16);
    ubo_Lights.AddUniform("lights[0].ambient_color", 16, 12);
    ubo_Lights.AddUniform("lights[0].diffuse_color", 32, 12);
    ubo_Lights.AddUniform("lights[0].specular_color", 48, 12);

    ubo_Lights.AddUniform("lights[1].position", 64, 16);
    ubo_Lights.AddUniform("lights[1].ambient_color", 80, 12);
    ubo_Lights.AddUniform("lights[1].diffuse_color", 96, 12);
    ubo_Lights.AddUniform("lights[1].specular_color", 112, 12);

    //ubo_Lights.SetUniform("lights[0].ambient_color", light_ambient);



    glm::vec3 camera_position = { 0.0, 0.0, 10.0 };
    glm::vec3 camera_look_at = { 0.0, 0.0, 0.0 };   // glm::lookAt(...) ??? check this out

    ux::CameraProperties cameraProperties = {
        camera_position,
        camera_look_at
    };

    auto ubo_Cameras = UniformBuffer("CameraProperties", 5, 32, &cameraProperties);
    ubo_Cameras.AddUniform("cameras[0].location", 0, 12);
    ubo_Cameras.AddUniform("cameras[0].look_at", 16, 12);

    //ubo_CameraProperties.AddUniformVec3("cameras", 0, "position", 0, 12);

    //ubo_CameraProperties.SetUniform("cameras[0].camera_location", camera_position);
    //ubo_CameraProperties.SetUniformVec3("cameras", 0, "position", camera_position);




    float zNear = 0.01f;
    float zFar = 2000.0;


    float gamma = 1.0;



 

   

    glm::vec4 ambient_color = { 0.348f, 0.348f, 0.348f, 1.0f };
    glm::vec4 diffuse_color = { 0.608f, 0.608f, 0.608f, 1.0f };
    glm::vec4 specular_color = { 0.5f, 0.5f, 0.5f, 1.0f };
    //float ambient_strength = 0.1;
    //float specular_strength = 0.5;
    float specular_shininess = 32.0;

    /*
    ux::Material material = {
        ambient_color,
        diffuse_color,
        specular_color,
        specular_shininess
    };
    */

    ux::Material material = {
        { 0.348f, 0.348f, 0.348f, 1.0f },    // ambient color
        { 0.608f, 0.608f, 0.608f, 1.0f },    // diffuse color
        { 0.5f, 0.5f, 0.5f, 1.0f },          // specular color
        32.0f                                // specular shininess
    };

    std::cout << "Material Size: " << sizeof(material) << std::endl;

    auto ubo_Materials = UniformBuffer("MaterialProperties", 4, 64, &material);
    ubo_Materials.AddUniform("material.ambient_color", 0, 12);
    ubo_Materials.AddUniform("material.diffuse_color", 16, 12);
    ubo_Materials.AddUniform("material.specular_color", 32, 12);
    ubo_Materials.AddUniform("material.specular_shininess", 48, 4);

    //ubo_Material.SetUniform("material.ambient_color", ambient_color);
    //ubo_Material.SetUniform("material.specular_color", specular_color);






    //glm::mat4 proj = glm::perspective(glm::radians(55.0f), window.GetAspectRatio(), zNear, zFar); // PROJECTION (SCREEN)
    glm::mat4 proj = glm::infinitePerspective(glm::radians(55.0f), window.GetAspectRatio(), zNear); // looks the same
    // also look at glm::frustum(...) matrix, glm::perspectiveFov(...)

    glm::mat4 viewBase = glm::lookAt(camera_position,     // Camera position in world space
        camera_look_at,      // look at origin
        glm::vec3(0, 1, 0)); // Head is up (set to 0, -1, 0 to look upside down)

    glm::mat4 viewTranslate = glm::translate(viewBase, glm::vec3(0.0, 0.0, translateZ));
    glm::mat4 viewRotateX = glm::rotate(viewTranslate, rotateXY.x, glm::vec3(1.0, 0.0, 0.0));
    glm::mat4 view = glm::rotate(viewRotateX, rotateXY.y, glm::vec3(0.0, 1.0, 0.0)); // CAMERA


    ux::SceneProperties scenes[1] = {
    
        glm::mat4(1.0f),
        view,
        proj,
        gamma
    };


    auto ubo_SceneProperties = UniformBuffer("SceneProperties", 6, 208, &scenes);
    ubo_SceneProperties.AddUniform("scenes[0].model", 0, 64);
    ubo_SceneProperties.AddUniform("scenes[0].view", 64, 64);
    ubo_SceneProperties.AddUniform("scenes[0].proj", 128, 64);
    ubo_SceneProperties.AddUniform("scenes[0].gamma", 192, 4);


   

    Renderer renderer;




 


    



    auto lightCube = ux::Cube(glm::vec3(1.0f, 1.0f, 1.0f));
    auto lightCube2 = ux::Cube(glm::vec3(1.0f, 1.0f, 1.0f));



    auto lines = ux::Lines();
    //lines.add(glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    //lines.add(glm::vec3(0.5, 0, 0), glm::vec3(0.5, 1, 0));
    lines.add(glm::vec3(0.0f, window.GetFramebufferWidth() - 1, 0.0f), glm::vec3(window.GetFramebufferWidth() - 1, window.GetFramebufferHeight() - 1, 0.0f)); // TOP
    lines.add(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(window.GetFramebufferWidth() - 1, 0.0f, 0.0f)); // BOTTOM
    lines.add(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, window.GetFramebufferHeight() - 1, 0.0f)); // LEFT
    lines.add(glm::vec3(window.GetFramebufferWidth() - 1, 0.0f, 0.0f), glm::vec3(window.GetFramebufferWidth() - 1, window.GetFramebufferHeight() - 1, 0.0f)); // RIGHT
    lines.commit();

    auto lines2 = ux::Lines();
    lines2.createCircle(2.0, 360);

    auto ring = ux::Lines();
    ring.createRing(2.2, 2.3, 0.1, 180);

    auto triangles = ux::Ring(2.2, 2.3, 0.1, 180);
    triangles.Build();



 
    auto textList = ux::TextList(50, 50, window.GetFramebufferWidth(), window.GetFramebufferHeight());
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
    std::array<ux::Ref<ux::Cube>, 260> sub_cubes;

    size_t sub_cube_count = 20;
    for (size_t n = 0;n < sub_cube_count;n++)
    {
        float angle = ux::PI2 / sub_cube_count * n;
        ux::Ref<ux::Cube> sub_cube1 = ux::CreateRef<ux::Cube>(glm::vec3(1.0));
        glm::mat4 model3 = glm::scale(glm::mat4(1.0), glm::vec3(1.0));
        model3 = ux::rotateAroundAxis(model3, axis, glm::vec3(0.0, angle, 0.0));
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
        glm::mat4 model4 = glm::scale(glm::mat4(1.0), glm::vec3(0.25));
        //glm::vec3 ss = glm::scale(glm::mat4(1.0), glm::vec3(1.0));
        //glm::vec4 obj = glm::vec4(0);
        //glm::translate();
        //glm::vec3 objTrans = 

        model4 = ux::rotateAroundAxis(model4, axis, glm::vec3(0.0, angle, 0.0));
        model4 = glm::translate(model4, glm::vec3(11.5f, 0, 0)); // radius       

        //glm::mat4 trans = glm::translate(glm::mat4(1), translate * scale);
        //model4 = trans * rotate(model4, axis, mRotate);

        sub_cube2->Transformation() = model4;
        sub_cubes[n + 20] = sub_cube2;
    }
    sub_cube_count = 180;
    for (size_t n = 0;n < sub_cube_count;n++)
    {
        float length = ux::random(8.0);
        float angle = ux::random(ux::RADIANS);

        glm::mat4 rm4 = ux::rotateAroundAxis(glm::mat4(1.0), axis, glm::vec3(0.0, angle, 0.0));
                  rm4 = glm::translate(rm4, glm::vec3(4, 0, 0)); // radius

        float radius = 0.05 + ux::random(0.1);
        ux::Ref<ux::Cube> sub_cube2 = ux::CreateRef<ux::Cube>(glm::vec3(0.0, -radius, -radius), glm::vec3(0.5 + length, radius, radius), rm4);

        sub_cube2->Transformation() = glm::mat4(1.0);
        sub_cubes[n + 80] = sub_cube2;
    }


    
    const int moreCount = 300;
    std::array<ux::Ref<ux::Cuboid>, moreCount> moreCubes;
    for (size_t n = 0;n < moreCount;n++)
    {
        float length = ux::random(5.0);
        float angle = ux::random(ux::RADIANS);

        glm::mat4 rm4 = ux::rotateAroundAxis(glm::mat4(1.0), axis, glm::vec3(0.0, angle, 0.0));
        //glm::mat4 rm4 = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 1.0f, 0.0f));
        rm4 = rm4 * glm::translate(rm4, glm::vec3(8, 0, 0)); // radius
        //glm::mat4 rm4 = glm::translate(glm::mat4(1.0f), glm::vec3(25, 0, 0)); // radius
        //rm4 = rm4 * glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 1.0f, 0.0f));
        //rm4 = rm4 * ux::rotateAroundAxis(glm::mat4(1.0), axis, glm::vec3(0.0, angle, 0.0));

        float radius = 0.025 + ux::random(0.1);
        //ux::Ref<ux::Cuboid> moreCube = ux::CreateRef<ux::Cuboid>(glm::vec3(0.0, -radius, -radius), glm::vec3(0.5 + length, radius, radius), rm4);
        ux::Ref<ux::Cuboid> moreCube = ux::CreateRef<ux::Cuboid>(glm::vec3(0.0, -radius, -radius), glm::vec3(0.5 + length, radius, radius), glm::mat4(1.0f));
        moreCube->Build();

        //ux::Ref<ux::Cuboid> moreCube = ux::CreateRef<ux::Cuboid>();
        moreCube->SetTransformation(rm4); //glm::mat4(1.0);
        moreCubes[n] = moreCube;
    }

    

    //glm::mat4 tfm = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 0.0f, 0.0f));
    glm::mat4 tfm = glm::mat4(1.0f);
    auto cuboid = ux::Cuboid(glm::vec3(-0.2f), glm::vec3(0.2f), tfm);
    cuboid.Build();




    glm::vec4 amber = ux::COLOR_AMBER;
    amber.a = 0.8;

    float yy = window.GetFramebufferHeight() - 74;
    auto horzBar = ux::HorzBar(glm::vec2(200.0f, yy - 20), glm::vec2(400.0f, yy));
    horzBar.SetColor(amber);
    horzBar.Build();
   

 
    glm::mat4 modelLight = glm::translate(glm::mat4(1.0), lightPos);

    //shaderBase.Bind();
    shaderBase.SetUniformMat4f("u_proj", proj);
    shaderBase.SetUniformMat4f("u_view", view);
    shaderBase.SetUniformMat4f("u_model", modelLight);

    //shaderBase2.Bind();
    shaderBase2.SetUniformMat4f("u_proj", proj);
    shaderBase2.SetUniformMat4f("u_view", view);
    shaderBase2.SetUniformMat4f("u_model", modelLight);


    // ORTHO
    glm::mat4 projOrtho = glm::ortho(0.f, (float)window.GetFramebufferWidth(), 0.0f, (float)window.GetFramebufferHeight(), 0.1f, zFar); // PROJECTION (SCREEN)
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





    
    ux::Mesh emitterMesh = ux::Mesh();
    emitterMesh.Load("Cube_Cube.003", "res/meshes/emitter.obj");
    //emitterMesh.Load("head_Cube", "res/meshes/Toon-Female-blockout.obj");
    emitterMesh.Build();
    std::cout << "MESH count: " << emitterMesh.GetVertexCount() << std::endl;

    


    /*
    entt::registry registry;

    auto entity = registry.create();

    registry.emplace<glm::vec3>(entity, 10.0f, 10.0f, 10.0f);

    glm::vec3 out = registry.get<glm::vec3>(entity);

    std::cout << "OUT: " << glm::to_string(out) << std::endl;

    registry.destroy(entity);
    */


    bool MULTI_SAMPLE = true;
    int samples = 8;

    auto frameBufferShader = Shader("res/shaders/framebuffer.shader");
    frameBufferShader.Bind();
    frameBufferShader.SetUniform1i("screenTexture", 0);
    

    //FrameBufferSpecification spec;
    //spec.Width = 1500;
    //spec.Height = 900;
    //FrameBuffer frameBuffer = FrameBuffer(spec);


    float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
       // positions   // texCoords
       -1.0f,  1.0f,  0.0f, 1.0f,
       -1.0f, -1.0f,  0.0f, 0.0f,
        1.0f, -1.0f,  1.0f, 0.0f,

       -1.0f,  1.0f,  0.0f, 1.0f,
        1.0f, -1.0f,  1.0f, 0.0f,
        1.0f,  1.0f,  1.0f, 1.0f
    };
    // screen quad VAO
    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));


    UX_LOG_DEBUG("WINDOW: %dx%d", window.GetWidth(), window.GetHeight());

    // framebuffer configuration
    // -------------------------
    unsigned int framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    // create a color attachment texture
    unsigned int textureColorbuffer, m_DepthAttachment;
    //glGenTextures(1, &textureColorbuffer);  
    if (MULTI_SAMPLE)
    {
        glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &textureColorbuffer);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, textureColorbuffer);
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGBA8, window.GetWidth(), window.GetHeight(), GL_FALSE);
        //glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        //glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, textureColorbuffer, 0);


        glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &m_DepthAttachment);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_DepthAttachment);
        glTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_DEPTH24_STENCIL8, window.GetWidth(), window.GetHeight(), GL_FALSE);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
        //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, m_DepthAttachment, 0); // REMOVE
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, m_DepthAttachment, 0);
    }
    else {
        glCreateTextures(GL_TEXTURE_2D, 1, &textureColorbuffer);
        glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, window.GetWidth(), window.GetHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, NULL); // GL_RGBA?
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);

        glCreateTextures(GL_TEXTURE_2D, 1, &m_DepthAttachment);
        glBindTexture(GL_TEXTURE_2D, m_DepthAttachment);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, window.GetWidth(), window.GetHeight(), 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachment, 0);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, m_DepthAttachment, 0);
    }
    
    // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
    /*
    unsigned int fboMsaaId;
    if (MULTI_SAMPLE)
    {
        unsigned int rboC, rboD;
        glGenRenderbuffers(1, &rboC);
        glBindRenderbuffer(GL_RENDERBUFFER, rboC);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_RGBA8, window.GetWidth(), window.GetHeight());
        //glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rboC);

        glGenRenderbuffers(1, &rboD);
        glBindRenderbuffer(GL_RENDERBUFFER, rboD);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH_COMPONENT, window.GetWidth(), window.GetHeight());
        // GL_DEPTH_COMPONENT GL_DEPTH24_STENCIL8
        //glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboD);
        
        
        glGenFramebuffers(1, &fboMsaaId);
        glBindFramebuffer(GL_FRAMEBUFFER, fboMsaaId);
        //glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rboC);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboD);
    }
    else {
        unsigned int rbo;
        glGenRenderbuffers(1, &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, window.GetWidth(), window.GetHeight());
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
    }
    */
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        UX_LOG_ERROR("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
    else UX_LOG_DEBUG("FB GOOD!");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    //std::cout << "TextureColorBuffer: " << textureColorbuffer << std::endl;




    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //glLineWidth(2.5f);



    auto scoopedCorner = ux::ScoopedCorner(glm::vec2(10.0f, 10.0f), 50.0f, amber);
    scoopedCorner.Build();


  

    // TODO put in the renderer init?
    glPointSize(5.0);
    glEnable(GL_MULTISAMPLE);
    //glDisable(GL_MULTISAMPLE);
    glEnable(GL_LINE_SMOOTH);
    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);
    // Make sure to turn backface culling off (glDisable(GL_CULL_FACE) ) because since we can 
    // look through the mesh, we could see that it has no “back” face.
    glDisable(GL_CULL_FACE);
    //glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (use_imgui)
    {
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        const char* glsl_version = "#version 440";
        ImGui::CreateContext();
        //ImGuiIO& io = ImGui::GetIO(); (void)io;
        // Setup Platform/Renderer bindings
        ImGui_ImplGlfw_InitForOpenGL(window.GetNativeWindow(), true);
        ImGui_ImplOpenGL3_Init(glsl_version);
        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsClassic();
    }

    //glDisable(GL_MULTISAMPLE);

    // Loop until the user closes the window
    while (window.Loop())
    {
        if (sound1) {
            auto soundPos = irrklang::vec3df(lightPos.x, lightPos.y, lightPos.z);
            sound1->setPosition(soundPos);
        }

   

        if (use_render_buffer)
        {
            //frameBuffer.Bind();
            //std::cout << "using framebuffer " << framebuffer << std::endl;
            glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
            //glBindFramebuffer(GL_FRAMEBUFFER, fboMsaaId);
            //glViewport(0, 0, window.GetWidth(), window.GetHeight());
            //glViewport(0, 0, 100, 100);
           
            //glDepthFunc(GL_LESS);
            //glEnable(GL_TEXTURE_2D);
            

            //glClearColor(1.0f, 1.0f, 0.0f, 1.0f);
            //glClear(GL_COLOR_BUFFER_BIT);
            //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }

        renderer.Clear();
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        //glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_MULTISAMPLE);
        glEnable(GL_LINE_SMOOTH);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        float time = static_cast<float>(glfwGetTime());
        shaderBox.SetUniform1f("u_time", time);

 
        // PERSPECTIVE

        glm::mat4 model = ux::transform(glm::mat4(1.0f), glm::vec3(scale, scale, scale), translate, mRotate);
        //glm::mat4 model = ux::transform(glm::mat4(1.0f), glm::vec3(1.0f), translate, mRotate);

        model = ux::transform(model, glm::vec3(scale2, scale2, scale2), translate2, rotate2);

        glm::mat4 mvp = proj * view * model;

        shaderBox.SetUniformMat4f("u_view", view);
        shaderBox.SetUniformMat4f("u_proj", proj);
        
        if (show_models)
        {
            shaderBox.SetUniformMat4f("u_model", model);
            //cuboid.Draw(renderer, shaderBox);
        }


        
        if (show_models)
        {           
            for (auto sub_cube : sub_cubes)
            {
                shaderBox.SetUniformMat4f("u_model", model * sub_cube->Transformation());
                sub_cube->Draw(renderer, shaderBox);
            }
        }


        
        
        for (auto moreCube : moreCubes)
        {
            // Performs the transformations after the geometry has been canculated.
            // Keeping the real-time rotation on the screen and stuff in sync.
            //glm::mat4 transform = glm::mat4(0.0f); //moreCube->Transformation();
            //transform = glm::translate(glm::mat4(1), translate * scale);
            //
            /*
            glm::mat4 transform = moreCube->Transformation();
            glm::mat4 trans = glm::translate(glm::mat4(1), translate * scale);
            transform = trans * ux::rotateAroundAxis(transform, axis, mRotate);
            transform = glm::rotate(transform, rotator, glm::vec3(0, 1.0, 0));
            */
            //transform = glm::rotate(transform, mRotate[1], glm::vec3(0, 1.0, 0));
            //glm::mat4 transform = trans * ux::rotateAroundAxis(transform, axis, mRotate);
            //transform = ux::rotateAroundAxis(transform, axis, mRotate);

            //transform = glm::rotate(transform, rotator, glm::vec3(0, 1.0, 0));
            shaderBox.SetUniformMat4f("u_model", model * moreCube->GetTransformation());
            moreCube->Draw(renderer, shaderBox);
        }
        

        if (show_models)
        {
            shaderBox.SetUniformMat4f("u_model", model);
            emitterMesh.Draw(renderer, shaderBox);
        }


        if (show_models)
        {
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
        }
        


        // note this used shaderBase!!!
        lightCube.Draw(renderer, shaderBase);
        lightCube2.Draw(renderer, shaderBase2);
        



        //shaderQuad.Bind();
        //shaderQuad.SetUniformMat4f("u_MVP", mvp);
        //shaderQuad.SetUniform4f("u_Color", 1.0f, 1.0f, 1.0f, 1.0f);
              
        // wireframe cube
        //cube.DrawOutline(renderer, shaderQuad);
        //ring.Draw(renderer, shaderQuad);


        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // border
        shaderQuad.SetUniformMat4f("u_MVP", mvpOrtho);
        shaderQuad.SetUniform4f("u_Color", 1.0f, 1.0f, 1.0f, 1.0f);
        lines.Draw(renderer, shaderQuad);
        

        shaderUX.SetUniformMat4f("u_MVP", mvpOrtho);
        shaderUX.SetUniform4f("u_Color", 1.0f, 1.0f, 1.0f, 1.0f);    
        scoopedCorner.Draw(renderer, shaderUX);
        horzBar.Draw(renderer, shaderUX);
        lines2.Draw(renderer, shaderUX); // why won't it draw if this isn't here?

        



        if (show_text)
        {
            // TODO text can be made faster here!
            textList.SetText(2, ux::format("MODEL: % 12.6f % 12.6f % 12.6f", mRotate[0], mRotate[1], mRotate[2]));
            textList.SetText(3, ux::format("LIGHT: % 12.6f % 12.6f % 12.6f", lightPos[0], lightPos[1], lightPos[2]));
            textList.SetText(4, ux::format("COLOR: %6.4f %1.4f %1.4f %1.4f", color[0], color[1], color[2], color[3]));
            float msFrame = 1000.0f / ImGui::GetIO().Framerate;
            float frameRate = ImGui::GetIO().Framerate;
            textList.SetText(5, ux::format("MS/FR: %8.6f", msFrame));
            textList.SetText(6, ux::format("  FPS: %8.3f", frameRate));
            textList.SetText(7, ux::ReadableTime());

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
                ImGui::SliderFloat3("M1 Rotate", (float*)&mRotate, 0.0f, ux::PI2);
                ImGui::Separator();
                ImGui::SliderFloat("M2 Scale", &scale2, 1.0f, 50.0f);
                ImGui::SliderFloat3("M2 Translate", (float*)&translate2, -20.0f, 20.0f);
                ImGui::SliderFloat3("M2 Rotate", (float*)&rotate2, 0.0f, ux::PI2);
                ImGui::Separator();
                // MATERIAL
                ImGui::ColorEdit3("Ambient Color", (float*)&material.ambient_color, misc_flags);
                if (ImGui::IsItemActive()) ubo_Materials.SetUniformVec3("material.ambient_color", material.ambient_color);
                ImGui::ColorEdit3("Diffuse Color", (float*)&material.diffuse_color, misc_flags);
                if (ImGui::IsItemActive()) ubo_Materials.SetUniformVec3("material.diffuse_color", material.diffuse_color);
                ImGui::ColorEdit3("Specular Color", (float*)&material.specular_color, misc_flags);
                if (ImGui::IsItemActive()) ubo_Materials.SetUniformVec3("material.specular_color", material.specular_color);
                ImGui::SliderFloat("Specular Shininess", &material.specular_shininess, 0.0f, 100.0f);
                if (ImGui::IsItemActive()) ubo_Materials.SetUniform1f("material.specular_shininess", material.specular_shininess);
                ImGui::Separator();

                ImGui::SliderFloat3("Camera Position", (float*)&camera_position, -100.0f, 100.0f);
                ImGui::SliderFloat3("Camera Look At", (float*)&camera_look_at, -100.0f, 100.0f);
                ImGui::SliderFloat("Z Near", &zNear, -200.0f, 2000.0f);
                ImGui::SliderFloat("Z Far", &zFar, 0.0f, 2000.0f);
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
                ImGui::SliderFloat("Gamma", &gamma, 0.0f, 5.0f);
                if (ImGui::IsItemActive()) ubo_SceneProperties.SetUniform1f("scenes[0].gamma", gamma);
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

            glm::vec3 lp2 = glm::vec3(1.0f,lightPos.y, lightPos.x);
            //glm::mat4 lp2m = ux::rotateAroundAxis(glm::mat4(1.0f), axis, mRotate);

            ubo_Lights.SetUniformVec4("lights[1].position", glm::vec4(lp2, 1.0f));
            shaderBase2.Bind();
            shaderBase2.SetUniformMat4f("u_model", glm::translate(glm::mat4(1.0), lp2));
            //shaderBase2.SetUniformMat4f("u_model", lp2m);
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

        
        if (use_render_buffer)
        {
            //frameBuffer.Unbind();
            if (MULTI_SAMPLE)
            {
                //glBindFramebuffer(GL_READ_FRAMEBUFFER, fboMsaaId);
                //glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);
                //glBlitFramebuffer(0, 0, window.GetWidth(), window.GetHeight(), 0, 0, window.GetWidth(), window.GetHeight(), GL_COLOR_BUFFER_BIT, GL_NEAREST);

            }
            // now bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            
            
            
            // clear all relevant buffers
            glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // set clear color to white (not really necessery actually, since we won't be able to see behind the quad anyways)
            //glClear(GL_COLOR_BUFFER_BIT);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glDisable(GL_DEPTH_TEST);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            frameBufferShader.Bind();
            glActiveTexture(GL_TEXTURE0); // + slot WOW guy didn't do this learnopengl

            //frameBufferShader.SetUniform1i("screenTexture", textureColorbuffer);
            glBindVertexArray(quadVAO);

            if (MULTI_SAMPLE)
            {
                
                //glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
                glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, textureColorbuffer);
                glDrawArrays(GL_TRIANGLES, 0, 6);
            }
            else {
                glBindTexture(GL_TEXTURE_2D, textureColorbuffer);	// use the color attachment texture as the texture of the quad plane
                glDrawArrays(GL_TRIANGLES, 0, 6);
            }
            //GLenum err = glGetError();
            //std::cout << "GL Error: " << err << std::endl;
        }
        
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

    //glfwDestroyWindow(window);
    //glfwTerminate();
    window.Close();
    
    return 0;
}