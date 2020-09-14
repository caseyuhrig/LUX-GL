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

#define GLM_FORCE_SWIZZLE
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "lux/Window.hpp"
#include "lux/Primitive/Cube.h"
#include "lux/Primitive/Segment.hpp"
#include "lux/Primitive/Plane.hpp"
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
#include "lux/Scene/Scene.hpp"
#include "lux/Scene/Camera.hpp"
#include "lux/Scene/Canvas.hpp"
#include "lux/Scene/Skybox.hpp"
#include "lux/Scene/Shadows.hpp"
#include "lux/Renderer/UniformBuffer.hpp"
#include "lux/Renderer/Timer.hpp"
#include "lux/Time.hpp"
#include "lux/StringUtils.hpp"
#include "lux/MatrixUtils.hpp"
#include "lux/Scene/LayerStack.hpp"
#include "lux/Scene/ImGuiLayer.hpp"
#include "lux/Scene/LcarsLayer.hpp"


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

   // https://www.youtube.com/watch?v=5HWCsmE9DrE
    //std::async(std::launch::async, [] {});

    //void* someFunc = []() { 1 + 1; };

    //std::function<void()> someFunc = [=]() -> void {
    //    std::cout << "Hello world!" << std::endl;
    //};

    //someFunc();


    // TODO, add the timestep and canvas into the window.Loop()

    // while(window.Render()) {} ???
    // while (renderer.Running()) {}
    // while (renderer.Rendering()) {}

    //glm::mat4 model = lux::MatrixUtils::Transform(glm::mat4(1.0f), glm::vec3(scale, scale, scale), translate, mRotate);
    //model = lux::MatrixUtils::Transform(model, glm::vec3(scale2, scale2, scale2), translate2, rotate2);
    //glm::mat4 mvp = camera.GetViewProjection() * model;


int main(int argc, char** argv)
{
    auto window = lux::Window("LUX/GL", 1500, 900);
    window.Center();




    // REGISTRY -----------------------------------------------
    entt::registry registry;



    // lighting
    glm::vec3 lightInc = glm::vec3(1.0, 0.0, 0.0);

   

  

    lux::LightProperties lightProps[2] = {
        lux::LightProperties(
        glm::vec3(13.0, 17.0, 1.0),    // color
        glm::vec4(0.2, 0.2, 0.2, 1.0), // ambient
        glm::vec4(0.5, 0.5, 0.5, 1.0), // diffuse
        glm::vec4(0.2, 0.2, 0.2, 1.0)  // specular
    ),
        lux::LightProperties(
        glm::vec3(1.0f, 1.0f, -24.0f), // color
        glm::vec4(0.2, 0.2, 0.2, 1.0), // ambient
        glm::vec4(0.5, 0.5, 0.5, 1.0), // diffuse
        glm::vec4(0.2, 0.2, 0.2, 1.0)  // specular
    )
    };

  
    auto lights = lux::Lights(lightProps);

    auto lightCube = lux::Cube(glm::vec3(1.0f, 1.0f, 1.0f));
    auto lightCube2 = lux::Cube(glm::vec3(1.0f, 1.0f, 1.0f));


    auto defaultMaterial = lux::Material();
  


    lux::Renderer renderer;
    renderer.SetClearColor({ 0.0f, 0.0f, 0.0f, 1.0f });

    


    uint32_t ACTIVE_CAMERA = 0;

    auto camera = lux::Camera(glm::vec3(0.0f,0.0f,50.0f), glm::vec3(0.0f,0.0f,0.0f));
    //camera.SetAspectRatio(window.GetAspectRatio());
    camera.SetViewportSize(window.GetFramebufferWidth(), window.GetFramebufferHeight());
    camera.Publish();
    

    auto scene = lux::Scene(camera);
    scene.Publish();


    glm::vec3 axis = { 0.0, 0.0, 0.0 };




    for (size_t n = 0;n < 20;n++)
    {
        float angle = lux::PI2f / 20 * n;

        glm::mat4 model3 = glm::scale(glm::mat4(1.0), glm::vec3(1.0));
        model3 = lux::MatrixUtils::RotateAroundAxis(model3, axis, glm::vec3(0.0, angle, 0.0));
        model3 = glm::translate(model3, glm::vec3(10, 0, 0)); // radius   

        auto s_cube = lux::Cuboid(glm::vec3(-0.5), glm::vec3(0.5), glm::mat4(1));

        auto entity = registry.create();
        registry.emplace<lux::Mesh>(entity, s_cube);
        registry.emplace<glm::mat4>(entity, model3);
    }

    for (size_t n = 0;n < 60;n++)
    {
        float angle = lux::PI2f / 60 * n;
        
        glm::mat4 model4 = glm::scale(glm::mat4(1.0), glm::vec3(0.25));

        model4 = lux::MatrixUtils::RotateAroundAxis(model4, axis, glm::vec3(0.0, angle, 0.0));
        model4 = glm::translate(model4, glm::vec3(11.5f, 0, 0)); // radius             

        auto s_cube = lux::Cuboid(glm::vec3(-0.5), glm::vec3(0.5), glm::mat4(1));
        
        auto entity = registry.create();
        registry.emplace<lux::Mesh>(entity, s_cube);
        registry.emplace<glm::mat4>(entity, model4); // *sub_cube2->Transformation());
    }
    
    for (size_t n = 0;n < 180;n++)
    {
        float length = lux::random(8.0f);
        float angle = lux::random(lux::RADIANSf);

        glm::mat4 rm4 = lux::MatrixUtils::RotateAroundAxis(glm::mat4(1.0), axis, glm::vec3(0.0, angle, 0.0));
                  rm4 = glm::translate(rm4, glm::vec3(4, 0, 0)); // radius

        float radius = 0.05 + lux::random(0.1f);

        auto s_cube = lux::Cuboid(glm::vec3(0.0, -radius, -radius), glm::vec3(0.5 + length, radius, radius), glm::mat4(1));

        auto entity = registry.create();
        registry.emplace<lux::Mesh>(entity, s_cube);
        registry.emplace<glm::mat4>(entity, rm4);
    }
    
    for (size_t n = 0;n < 300;n++)
    {
        float length = lux::random(5.0f);
        float angle = lux::random(lux::RADIANSf);

        glm::mat4 rm4 = lux::MatrixUtils::RotateAroundAxis(glm::mat4(1.0), axis, glm::vec3(0.0, angle, 0.0));
        rm4 = rm4 * glm::translate(rm4, glm::vec3(8, 0, 0)); // radius

        float radius = 0.025f + lux::random(0.1f);

        auto moreCube = lux::Cuboid(glm::vec3(0.5f, -radius, -radius), glm::vec3(0.5f + length, radius, radius), glm::mat4(1.0f));

        auto entity = registry.create();
        registry.emplace<lux::Mesh>(entity, moreCube);
        registry.emplace<glm::mat4>(entity, rm4);
    }

    // the angled ones
    for (size_t n = 0;n < 100;n++)
    {
        float length = lux::random(5.0f);
        float angle = lux::random(lux::RADIANSf);
        float radius = lux::random(0.1f);

        auto bar = lux::Cuboid(glm::vec3(0.0, -radius, -radius), glm::vec3(0.5 + length, radius, radius), glm::mat4(1.0f));

        glm::mat4 rm4 = lux::MatrixUtils::RotateAroundAxis(glm::mat4(1.0), axis, glm::vec3(0.0, angle, 0.2));
        rm4 = glm::translate(rm4, glm::vec3(4, 0, 0));

        auto entity = registry.create();
        registry.emplace<lux::Mesh>(entity, bar);
        registry.emplace<glm::mat4>(entity, rm4);
    }
    
   

    auto cube = lux::Cuboid(glm::vec3(-0.5), glm::vec3(0.5), glm::mat4(1.0));
    {
        auto entity = registry.create();
        registry.emplace<lux::Mesh>(entity, cube);
        registry.emplace<glm::mat4>(entity, glm::mat4(1));
    }

    auto shaderBox = lux::Shader("res/shaders/box.shader");

    // used for the lights
    auto shaderBase = lux::Shader("res/shaders/base.shader");
    //auto shaderBase2 = lux::Shader("res/shaders/base.shader");




    auto shaderText = lux::Shader("res/shaders/text.shader");
    


 
    

    shaderBase.Bind();
    shaderBase.SetUniformMat4f("u_proj", camera.GetProjection());
    shaderBase.SetUniformMat4f("u_view", camera.GetView());
   


   

    
    lux::Mesh emitterMesh = lux::Mesh();
    emitterMesh.Load("Cube_Cube.003", "res/meshes/emitter.obj");
    //emitterMesh.Load("head_Cube", "res/meshes/Toon-Female-blockout.obj");
    emitterMesh.Build();
    {
        //auto entity = registry.create();
        //registry.emplace<lux::Mesh>(entity, emitterMesh);
        //registry.emplace<glm::mat4>(entity, glm::mat4(1));
    }

  
    

 


    auto canvas = lux::Canvas(window.GetWidth(), window.GetHeight(), 8);


    auto skybox = lux::Skybox();
    skybox.SetCamera(camera);
    {
        auto entity = registry.create();
        registry.emplace<lux::Skybox>(entity, skybox);
        registry.emplace<glm::mat4>(entity, glm::mat4(1));
    }

    //auto cubemapShader = lux::Shader("res/shaders/cubemap-shader.glsl");
    auto cubemapShader = lux::Shader("res/shaders/cubemap-lighting-shader.glsl");

    //auto testShader = cubemapShader;
    auto testShader = shaderBox;




    lux::ImGuiLayer imguiLayer = lux::ImGuiLayer(&window);
    imguiLayer.SetCamera(&camera);
    imguiLayer.SetMaterial(&defaultMaterial);
    imguiLayer.SetLights(&lights);
    imguiLayer.SetShaderBase(&shaderBase);
    glm::vec3 lightPos = lights.GetPosition(0).xyz;
    imguiLayer.SetLightPos(&lightPos);

    float scale = 1.0f; // 5.0 1.0; //0.125;  
    glm::vec3 translate = { 0.0, 0.0, 0.0 };
    glm::vec3 mRotate = { 0.0, 0.0, 0.0 };
    float translateZ = -100.0;
    glm::vec2 rotateXY = glm::vec2(0.0, glm::radians(0.0));

    float scale2 = 1.0f;
    glm::vec3 translate2 = { 0.0f, 0.0f, 0.0f };
    glm::vec3 rotate2 = { 0.0f, 0.0f, 0.0f };

    glm::vec3 scaleCube = glm::vec3(1.0);
    glm::vec3 translateCube = glm::vec3(0.0);
    glm::vec3 rotateCube = glm::vec3(0.0);

    imguiLayer.SetCubeTransforms(&scaleCube, &translateCube, &rotateCube);

    imguiLayer.Set1(&scale, &translate, &mRotate);
    imguiLayer.Set2(&scale2, &translate2, &rotate2);
    //imguiLayer.SetCubemapSwapFunction([testShader, cubemapShader] mutatable {
    //    testShader = cubemapShader;
    //});

    
    int shadow_samples = 10;
    float shadow_bias = 0.3;

    imguiLayer.SetShadowSamples(&shadow_samples);
    imguiLayer.SetShadowBias(&shadow_bias);

    lux::LcarsLayer lcarsLayer = lux::LcarsLayer(&window, renderer, mRotate);
    lcarsLayer.SetLights(lights);
    lcarsLayer.SetRotate(mRotate);


    // LAYERS + IMGUI
    lux::LayerStack layers = lux::LayerStack();  
    layers.Add(&imguiLayer);
    layers.Add(&lcarsLayer);
    layers.Begin();




    
    auto segment = lux::Segment(0.5f, 1.0f, 0.0f, 45.0f, 2.0f);
    for (float angle = 0.0; angle< 360.0; angle += 90.0)
    {
        auto entity = registry.create();
        registry.emplace<lux::Mesh>(entity, segment);
        registry.emplace<glm::mat4>(entity, glm::rotate(glm::mat4(1), angle * lux::TO_RADf, glm::vec3(0.0, 1.0, 0.0)));
    }


    auto segment2 = lux::Segment(1.25f, 1.5f, 0.0f, 15.0f, 1.0f);
    for (float angle = 0.0f;angle < 360.0f;angle += 20.0f)
    {
        auto entity = registry.create();
        registry.emplace<lux::Mesh>(entity, segment2);
        registry.emplace<glm::mat4>(entity, glm::rotate(glm::mat4(1), angle * lux::TO_RADf, glm::vec3(0.0, 1.0, 0.0)));
    }
    
    /*
    auto plane = lux::Plane(20, -5, 10);
    plane.SetColor(glm::vec4(1.0f));
    plane.Build();
    {
        auto entity = registry.create();
        registry.emplace<lux::Mesh>(entity, plane);
        registry.emplace<glm::mat4>(entity, glm::mat4(1));
    }
    */
    

 
    auto timestep = lux::Timer();


    testShader.SetUniformMat4f("u_view", camera.GetView());
    testShader.SetUniformMat4f("u_proj", camera.GetProjection());
    testShader.SetUniform1f("u_time", timestep);


    
    auto shadows = lux::Shadows(camera);
    //shadows.Init();
   
    // TODO move this out of the Shadows class, no reason to get the Shader from that location.
    //auto shader = shadows.GetShader();
    auto shader = lux::Shader("res/shaders/point_shadows.glsl");
    shader.SetUniform1i("shadows", true);
    // TODO, want to remove this, hmm?
    auto simpleDepthShader = shadows.GetDepthShader();

    auto shaderVisualizeNormals = lux::Shader("res/shaders/visualize_normals.glsl");
    
    //camera.SetViewportSize(window.GetFramebufferWidth(), window.GetFramebufferHeight());
    //camera.Publish();

    
  

    while (!window.ShouldClose())
    {
        glm::mat4 model = lux::MatrixUtils::Transform(glm::mat4(1.0f), glm::vec3(scale, scale, scale), translate, mRotate);
        model = lux::MatrixUtils::Transform(model, glm::vec3(scale2, scale2, scale2), translate2, rotate2);
        glm::mat4 mvp = camera.GetViewProjection() * model;

        
        renderer.Clear();

     

        // 1. Render everything that casts a shadow.
        shadows.Bind(lightPos);      
        {
            auto entityView = registry.view<lux::Mesh, glm::mat4>();
            for (auto entity : entityView)
            {
                auto& mesh = entityView.get<lux::Mesh>(entity);
                auto& transformation = entityView.get<glm::mat4>(entity);

                shadows.SetModelTransformation(model * transformation);
                //simpleDepthShader->SetUniformMat4f("model", model * transformation);
                mesh.Draw(renderer, *simpleDepthShader);
            }
        }
        shadows.UnBind();



        // 2. render scene as normal
        glViewport(0, 0, window.GetFramebufferWidth(), window.GetFramebufferHeight());

        canvas.Bind();
        renderer.Clear();
        // TODO use the timestep
        testShader.SetUniform1f("u_time", timestep);

        //cubemapShader.SetUniformMat4f("view", view);
        //cubemapShader.SetUniformMat4f("projection", proj);
        //cubemapShader.SetUniformMat4f("model", model * sub_cube->Transformation());

        shader.Bind();
        
        shader.SetUniformMat4f("projection", camera.GetProjection());
        shader.SetUniformMat4f("view", camera.GetView());
        // set lighting uniforms
        shader.SetUniformVec3f("lightPos", lightPos);
        shader.SetUniformVec3f("viewPos", camera.GetPosition());
        //shader->SetUniform1i("shadows", shadows);
        shader.SetUniform1f("far_plane", camera.GetZFar());

        shader.SetUniform1f("u_bias", shadow_bias);
        shader.SetUniform1i("u_samples", shadow_samples);

        //shader->BindAndActivateTexture(); // do the below 3 lines
        shader.SetUniform1i("depthMap", 1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, shadows.GetDepthCubemap());
        {
            auto entityView = registry.view<lux::Mesh, glm::mat4>();
            for (auto entity : entityView)
            {
                auto& mesh = entityView.get<lux::Mesh>(entity);
                auto& transformation = entityView.get<glm::mat4>(entity);

                shader.SetUniformMat4f("model", model * transformation);
                mesh.Draw(renderer, shader);

                /*
                shaderVisualizeNormals.SetUniformMat4f("projection", camera.GetProjection());
                shaderVisualizeNormals.SetUniformMat4f("view", camera.GetView());
                shaderVisualizeNormals.SetUniformMat4f("model", transform);
                mesh.Draw(renderer, shaderVisualizeNormals);
                */
            }
        }

        //shaderBase.SetUniformMat4f("u_model", glm::translate(glm::mat4(1.0), lights.GetPosition(0).xyz()));
        shaderBase.SetUniformMat4f("u_proj", camera.GetProjection());
        shaderBase.SetUniformMat4f("u_view", camera.GetView());
        shaderBase.SetUniformMat4f("u_model", glm::translate(glm::mat4(1.0), lightPos));
        //shader->SetUniformMat4f("model", glm::translate(glm::mat4(1.0), lightPos));

        //glEnable(GL_BLEND);
        //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        lightCube.Draw(renderer, shaderBase);
        //glDisable(GL_BLEND);

        //shaderBase.SetUniformMat4f("u_model", glm::translate(glm::mat4(1.0), lights.GetPosition(1).xyz()));
        //lightCube2.Draw(renderer, shaderBase);

        
        //glm::vec3 lightPos = lights.GetPosition(0);
        // 130 = distance, 4 = seconds
        /*
        lightPos.x = timestep.PingPong(lightPos.x, lightInc.x, -65.0f, 65.0f, 130.0f, 4.0f);
        lights.SetPosition(0, lightPos);
        glm::vec3 lp2 = glm::vec3(1.0f, lightPos.y, lightPos.x);
        lights.SetPosition(1, lp2);
        */

        layers.Draw();
       
        canvas.Unbind();




        renderer.Clear();

        canvas.Draw();

        window.SwapBuffers();

        timestep.Update();
    }

    layers.End();

    //registry.destroy(entity);
    //registry.destroy();

    window.Close();   

    return 0;
}