#include <glad/glad.h>  
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
#include <future>

//#define SPDLOG_HEADER_ONLY
//#define SPDLOG_COMPILED_LIB
//#include "spdlog/spdlog.h";
// some magic needed to get spdlog working!
// except it's not working now, build issues.

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "lux/Window.hpp"
#include "lux/Primitive/Cube.h"
#include "lux/Primitive/Segment.hpp"
#include "lux/Interface/Lines.hpp"
#include "lux/TextList.hpp"
#include "lux/Primitive/Cuboid.hpp"
#include "lux/Color.hpp"
#include "lux/Scene/Material.hpp"
#include "lux/Interface/HorzBar.hpp"
#include "lux/Interface/ScoopedCorner.hpp"
#include "lux/Interface/RoundedCorner.hpp"
#include "lux/Interface/TopCar.hpp"
#include "lux/Interface/Quad.hpp"
#include "lux/Primitive/Ring.hpp"
#include "lux/Scene/Camera.hpp"
#include "lux/Renderer/Canvas.hpp"
#include "lux/Scene/Skybox.hpp"
#include "lux/Renderer/UniformBuffer.hpp"
#include "lux/Time.hpp"
#include "lux/StringUtils.hpp"
#include "lux/MatrixUtils.hpp"
#include "lux/Scene/LayerStack.hpp"
#include "lux/Scene/ImGuiLayer.hpp"


#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
//#define IMAPP_IMPL
//#include "imgui/ImApp.h"
//#include "imgui/ImGuizmo.h"



#include "entt/entt.hpp"

#include <irrKlang/irrKlang.h>

#include "nlohmann/json.hpp"




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


int main(int argc, char** argv)
{
    auto window = lux::Window("LUX/GL", 1500, 900);
    window.Center();





  


    glm::vec3 axis = { 0.0, 0.0, 0.0 };

    
    float scale = 3.0f; // 5.0 1.0; //0.125;  
    glm::vec3 translate = { -20.0, 0.299, 0.0 };
    glm::vec3 mRotate = { 0.0, 0.0, 0.0 };
    float translateZ = -100.0;
    glm::vec2 rotateXY = glm::vec2(0.0, glm::radians(0.0));

    float scale2 = 1.0f;
    glm::vec3 translate2 = { 0.0f, 0.896f, 0.0f };
    glm::vec3 rotate2 = { 0.375f, 0.469f, 0.0f };
    

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




    auto defaultMaterial = lux::Material();
  


    lux::Renderer renderer;
    //renderer.SetClearColor({ 0.0f, 0.0f, 0.0f, 1.0f });

    


    uint32_t ACTIVE_CAMERA = 0;

    lux::Camera camera = lux::Camera(glm::vec3(0.0f,0.0f,50.0f), glm::vec3(0.0f,0.0f,0.0f));
    camera.SetAspectRatio(window.GetAspectRatio());
    camera.SetViewportSize(window.GetFramebufferWidth(), window.GetFramebufferHeight());
    //glm::mat4 view = camera.GetView();
    //glm::mat4 proj = camera.GetProjection();
    camera.Publish();


    

    


    lux::SceneProperties scenes[1] = {

           glm::mat4(1.0f),
           camera.GetView(),
           camera.GetProjection(),
           renderer.Gamma()
    };


    auto ubo_SceneProperties = lux::UniformBuffer("SceneProperties", 6, 208, &scenes);
    ubo_SceneProperties.AddUniform("scenes[0].model", 0, 64);
    ubo_SceneProperties.AddUniform("scenes[0].view", 64, 64);
    ubo_SceneProperties.AddUniform("scenes[0].proj", 128, 64);
    ubo_SceneProperties.AddUniform("scenes[0].gamma", 192, 4);




    



    auto lightCube = lux::Cube(glm::vec3(1.0f, 1.0f, 1.0f));
    auto lightCube2 = lux::Cube(glm::vec3(1.0f, 1.0f, 1.0f));


    /*
    auto lines = lux::Lines();
    lines.add(glm::vec3(0.0f, window.GetFramebufferWidth() - 1, 0.0f), glm::vec3(window.GetFramebufferWidth() - 1, window.GetFramebufferHeight() - 1, 0.0f)); // TOP
    lines.add(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(window.GetFramebufferWidth() - 1, 0.0f, 0.0f)); // BOTTOM
    lines.add(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, window.GetFramebufferHeight() - 1, 0.0f)); // LEFT
    lines.add(glm::vec3(window.GetFramebufferWidth() - 1, 0.0f, 0.0f), glm::vec3(window.GetFramebufferWidth() - 1, window.GetFramebufferHeight() - 1, 0.0f)); // RIGHT
    lines.commit();
    */
  
 
    auto textList = lux::TextList(110, 600, window.GetFramebufferWidth(), window.GetFramebufferHeight());
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




    std::array<lux::Ref<lux::Cube>, 260> sub_cubes;

    size_t sub_cube_count = 20;
    for (size_t n = 0;n < sub_cube_count;n++)
    {
        float angle = lux::PI2 / sub_cube_count * n;
        lux::Ref<lux::Cube> sub_cube1 = lux::CreateRef<lux::Cube>(glm::vec3(1.0));
        glm::mat4 model3 = glm::scale(glm::mat4(1.0), glm::vec3(1.0));
        model3 = lux::MatrixUtils::RotateAroundAxis(model3, axis, glm::vec3(0.0, angle, 0.0));
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

        model4 = lux::MatrixUtils::RotateAroundAxis(model4, axis, glm::vec3(0.0, angle, 0.0));
        model4 = glm::translate(model4, glm::vec3(11.5f, 0, 0)); // radius       

        sub_cube2->Transformation() = model4;
        sub_cubes[n + 20] = sub_cube2;
    }
    sub_cube_count = 180;
    for (size_t n = 0;n < sub_cube_count;n++)
    {
        float length = lux::random(8.0);
        float angle = lux::random(lux::RADIANS);

        glm::mat4 rm4 = lux::MatrixUtils::RotateAroundAxis(glm::mat4(1.0), axis, glm::vec3(0.0, angle, 0.0));
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

        glm::mat4 rm4 = lux::MatrixUtils::RotateAroundAxis(glm::mat4(1.0), axis, glm::vec3(0.0, angle, 0.0));
        rm4 = rm4 * glm::translate(rm4, glm::vec3(8, 0, 0)); // radius

        float radius = 0.025 + lux::random(0.1);
        lux::Ref<lux::Cuboid> moreCube = lux::CreateRef<lux::Cuboid>(glm::vec3(0.0, -radius, -radius), glm::vec3(0.5 + length, radius, radius), glm::mat4(1.0f));
        moreCube->Build();

        moreCube->SetTransformation(rm4);
        moreCubes[n] = moreCube;
    }

   


    auto shaderBox = lux::Shader("res/shaders/box.shader");

    // used for the lights
    auto shaderBase = lux::Shader("res/shaders/base.shader");
    auto shaderBase2 = lux::Shader("res/shaders/base.shader");

    auto shaderText = lux::Shader("res/shaders/text.shader");
    auto shaderUX = lux::Shader("res/shaders/ux.shader");


 
    glm::mat4 modelLight = glm::translate(glm::mat4(1.0), lightPos);

    shaderBase.Bind();
    shaderBase.SetUniformMat4f("u_proj", camera.GetProjection());
    shaderBase.SetUniformMat4f("u_view", camera.GetView());
    shaderBase.SetUniformMat4f("u_model", modelLight);

    shaderBase2.Bind();
    shaderBase2.SetUniformMat4f("u_proj", camera.GetProjection());
    shaderBase2.SetUniformMat4f("u_view", camera.GetView());
    shaderBase2.SetUniformMat4f("u_model", modelLight);


    // ORTHO   
    glm::mat4 projOrtho = glm::ortho(0.f, (float)window.GetFramebufferWidth(), 0.0f, (float)window.GetFramebufferHeight(), 0.01f, 2000.0f); // PROJECTION (SCREEN)
    glm::mat4 viewOrtho = glm::lookAt(glm::vec3(0, 0, 10.0),  // Camera position in world space (doesn't really apply when using an ortho projection matrix)
        glm::vec3(0, 0, 0),  // look at origin
        glm::vec3(0, 1, 0)); // Head is up (set to 0, -1, 0 to look upside down)       
    glm::mat4 modelOrtho = glm::mat4(1.0f); // MODEL
    glm::mat4 mvpOrtho = projOrtho * viewOrtho * modelOrtho;
    
    shaderUX.SetUniformMat4f("u_MVP", mvpOrtho);
    shaderUX.SetUniform4f("u_Color", 1.0f, 1.0f, 1.0f, 1.0f);



    /*
    std::string soundBackgroundFilename = "res/audio/tlsf/engineloop.wav";
    std::string soundFilename = "res/audio/tlsf/Laser1.wav";
    irrklang::ISoundEngine* engine = irrklang::createIrrKlangDevice();

    auto sound_camera_location = irrklang::vec3df(camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
    auto sound_camera_look_at = irrklang::vec3df(camera.GetLookAt().x, camera.GetLookAt().y, camera.GetLookAt().z);

    engine->setListenerPosition(sound_camera_location, sound_camera_look_at);
    engine->setSoundVolume(1.0f);
    engine->setDefault3DSoundMinDistance(10.0f); // [default] 1.0f

    auto soundPos = irrklang::vec3df(lightPos.x, lightPos.y, lightPos.z);
    irrklang::ISound* sound1 = NULL;
    */

    
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


    // 2D Primitive UI layer

    float yy = window.GetFramebufferHeight() - 152;

    glm::vec4 amber = lux::COLOR_AMBER;
    amber.a = 0.8;

    auto scoopedCorner = lux::ScoopedCorner(glm::vec2(10.0f, 10.0f), 50.0f, amber);
    scoopedCorner.Build();

    auto roundedCorner = lux::RoundedCorner(lux::CornerPosition::TopRight, glm::vec2(70.0f, 10.0f), 50.0f, amber);
    roundedCorner.Build();

    auto roundedCorner2 = lux::RoundedCorner(lux::CornerPosition::TopLeft, glm::vec2(130.0f, 10.0f), 50.0f, amber);
    roundedCorner2.Build();

    auto topCar = lux::TopCar(glm::vec2(10.0f, 100.0f), glm::vec2(900.0f, 890.0f), amber);
    topCar.Build();

    auto backgroundQuad = lux::Quad(glm::vec3(10.0f, 150.04, 0.0f), glm::vec2(200.0f, 200.0f), glm::vec4(1.0f, 0.0f, 0.0f, 0.0f));
    backgroundQuad.Build();



    auto skybox = lux::Skybox();
    skybox.Init();

    //auto cubemapShader = lux::Shader("res/shaders/cubemap-shader.glsl");
    auto cubemapShader = lux::Shader("res/shaders/cubemap-lighting-shader.glsl");

    //auto testShader = cubemapShader;
    auto testShader = shaderBox;



    lux::ImGuiLayer imguiLayer = lux::ImGuiLayer(&window);
    imguiLayer.SetCamera(&camera);
    imguiLayer.SetMaterial(&defaultMaterial);
    imguiLayer.SetLights(&light1, &ubo_Lights);
    imguiLayer.SetShaderBase(&shaderBase);
    imguiLayer.Set1(&scale, &translate, &mRotate);
    imguiLayer.Set2(&scale2, &translate2, &rotate2);
    //imguiLayer.SetCubemapSwapFunction([?] {
    //    testShader = cubemapShader;
    //});

    // LAYERS + IMGUI
    lux::LayerStack layers = lux::LayerStack();  
    layers.Add(&imguiLayer);
    layers.Begin();



  

    // https://www.youtube.com/watch?v=5HWCsmE9DrE
    //std::async(std::launch::async, [] {});


    testShader.SetUniformMat4f("u_view", camera.GetView());
    testShader.SetUniformMat4f("u_proj", camera.GetProjection());



    auto segment = lux::Segment(0.5f, 1.0f, 0.0f, 45.0f, 2.0f);
    auto segment2 = lux::Segment(1.25f, 1.5f, 0.0f, 15.0f, 1.0f);

  

    while (window.Loop())
    {
        /*
        if (sound1) {
            auto soundPos = irrklang::vec3df(lightPos.x, lightPos.y, lightPos.z);
            sound1->setPosition(soundPos);
        }
        */
   
        canvas.Begin();
        renderer.Clear();      
        
        glm::mat4 model = lux::MatrixUtils::Transform(glm::mat4(1.0f), glm::vec3(scale, scale, scale), translate, mRotate);
        model = lux::MatrixUtils::Transform(model, glm::vec3(scale2, scale2, scale2), translate2, rotate2);
        //glm::mat4 mvp = proj * view * model;
        glm::mat4 mvp = camera.GetViewProjection() * model;

       
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
        //emitterMesh.Draw(renderer, testShader);
        
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
        
        
        lightCube.Draw(renderer, shaderBase);
        lightCube2.Draw(renderer, shaderBase2);


        //backgroundQuad.Draw(renderer, shaderUX);
        scoopedCorner.Draw(renderer, shaderUX);
        roundedCorner.Draw(renderer, shaderUX);
        roundedCorner2.Draw(renderer, shaderUX);
        //horzBar.Draw(renderer, shaderUX);
        topCar.Draw(renderer, shaderUX);
        


        if (imguiLayer.useSkybox)
            skybox.Render(camera.GetPosition(), camera.GetProjection());
        

        if (imguiLayer.showText)
        {
            // TODO text can be made faster here!
            textList.SetText(2, fmt::sprintf("MODEL: % 12.6f % 12.6f % 12.6f", mRotate[0], mRotate[1], mRotate[2]));
            textList.SetText(3, fmt::sprintf("LIGHT: % 12.6f % 12.6f % 12.6f", lightPos[0], lightPos[1], lightPos[2]));
            textList.SetText(4, fmt::sprintf("COLOR: %6.4f %1.4f %1.4f %1.4f", color[0], color[1], color[2], color[3]));
            float msFrame = 1000.0f / ImGui::GetIO().Framerate;
            float frameRate = ImGui::GetIO().Framerate;
            textList.SetText(5, fmt::sprintf("MS/FR: %8.6f", msFrame));
            textList.SetText(6, fmt::sprintf("  FPS: %8.3f", frameRate));
            textList.SetText(7, lux::StringUtils::ToUppercase(lux::Time::Readable()));

            textList.Draw();
        }

        //backgroundQuad.Draw(renderer, shaderUX);

        layers.Draw();


        if (imguiLayer.animate)
        {
            lightPos += lightInc;

            if (lightPos.x > 65.0f || lightPos.x < -65.0f)
            {
                lightInc.x = -lightInc.x;
            }
            ubo_Lights.SetUniformVec4("lights[0].position", glm::vec4(lightPos, 1.0));
            shaderBase.Bind();
            shaderBase.SetUniformMat4f("u_model", glm::translate(glm::mat4(1.0), lightPos));

            glm::vec3 lp2 = glm::vec3(1.0f, lightPos.y, lightPos.x);

            ubo_Lights.SetUniformVec4("lights[1].position", glm::vec4(lp2, 1.0f));
            shaderBase2.Bind();
            shaderBase2.SetUniformMat4f("u_model", glm::translate(glm::mat4(1.0), lp2));

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
    //if (sound1) sound1->drop(); // release music stream.
    //engine->drop();

    layers.End();

    window.Close();   
    return 0;
}