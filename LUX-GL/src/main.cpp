#define GLM_FORCE_SWIZZLE
#include <windows.h>

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
#include "lux/Primitive/Segment.hpp"
#include "lux/Primitive/Plane.hpp"
#include "lux/Interface/Lines.hpp"
#include "lux/TextList.hpp"
#include "lux/Primitive/Cuboid.hpp"
#include "lux/Primitive/Tetrahedron.hpp"
#include "lux/Primitive/Dodecahedron.hpp"
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
#include "lux/Renderer/ShaderStorageBuffer.hpp"
#include "lux/Renderer/Timer.hpp"
#include "lux/Renderer/GBuffer.hpp"
#include "lux/Time.hpp"
#include "lux/StringUtils.hpp"
#include "lux/MatrixUtils.hpp"
#include "lux/Scene/LayerStack.hpp"
#include "lux/Scene/ImGuiLayer.hpp"
#include "lux/Scene/LcarsLayer.hpp"
#include "lux/Scene/Components.hpp"

#include "lux/Platform/Microsoft/Windows.hpp"
#include "lux/Interface.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
//#define IMAPP_IMPL
//#include "imgui/ImApp.h"
//#include "imgui/ImGuizmo.h"

#include "entt/entt.hpp"

//#include <assimp/Importer.hpp>
//#include <irrKlang/irrKlang.h>
//#include "nlohmann/json.hpp"




int main(int argc, char** argv)
{
    auto window = lux::Window("LUX/GL", 1500, 900);
    window.Center();
    window.FillWorkArea();

    lux::Renderer::Init();
    lux::Renderer::SetClearColor(lux::Colors::Black);


  

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

    
    glm::vec3 lightPos = lights.GetPosition(0).xyz;
    glm::vec3 lightInc = glm::vec3(1.0, 0.0, 0.0);

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

    const auto identity = glm::mat4(1.0);
    const auto tetrahedronTransform = glm::translate(identity, glm::vec3(0.0f, 7.0f, 0.0f));

    int shadow_samples = 10;
    float shadow_bias = 0.3f;
    auto useShadows = true;

    const auto gray = glm::vec4(0.45f, 0.45f, 0.45f, 1.0f);
    
    const auto axis = glm::vec3(0.0f, 0.0f, 0.0f);

    const auto brightPinkColor = lux::Colors::DeepPink * 2.0f;
    const ImGuiColorEditFlags imgui_misc_flags = ImGuiColorEditFlags_Float;




    auto& shader = lux::Renderer::ShaderLibrary().Get("PointShadows");
    shader->SetUniform1i("shadows", useShadows);
    shader->SetUniformVec4f("u_LightColor", lux::Colors::NeonPink);

    auto& shaderBase = lux::Renderer::ShaderLibrary().Get("Base");
    //shaderBase->SetUniformMat4f("u_proj", camera->GetProjection());
    //shaderBase->SetUniformMat4f("u_view", camera->GetView());
    shaderBase->SetUniformVec4f("u_Color", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    shaderBase->SetUniformVec4f("u_BrightColor", glm::vec4(1.5f, 1.5f, 1.5f, 1.0f));

    auto defaultMaterial = lux::Material(shaderBase);




  


    auto canvas = lux::Canvas(window.GetWidth(), window.GetHeight(), 8);
    
    auto camera = lux::Camera::Create(window.GetWidth(), window.GetHeight(), glm::vec3(0.0f, 0.0f, 50.0f), glm::vec3(0.0f, 0.0f, 0.0f));

    auto scene = lux::Scene::Create(camera);
    //scene->AddLight(light1);
    scene->Publish();

    auto shadows = lux::Shadows(scene);




    auto imguiLayer = lux::ImGuiLayer(&window);
    imguiLayer.SetCamera(camera);
    imguiLayer.SetMaterial(&defaultMaterial);
    imguiLayer.SetLights(&lights);
    imguiLayer.SetShaderBase(shaderBase);
    imguiLayer.SetLightPos(&lightPos);
    imguiLayer.SetCubeTransforms(&scaleCube, &translateCube, &rotateCube);
    imguiLayer.Set1(&scale, &translate, &mRotate);
    imguiLayer.Set2(&scale2, &translate2, &rotate2);
    imguiLayer.SetShadowSamples(&shadow_samples);
    imguiLayer.SetShadowBias(&shadow_bias);


    lux::LcarsLayer lcarsLayer = lux::LcarsLayer(window.GetWidth(), window.GetHeight(), mRotate);
    lcarsLayer.SetLights(lights);
    lcarsLayer.SetRotate(mRotate);

    lux::LayerStack layers = lux::LayerStack();  
    layers.Attach(&imguiLayer);
    layers.Attach(&lcarsLayer);
    



    auto& registry = scene->GetRegistry();
    
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
    
    //auto tetrahedron = lux::Tetrahedron();
    auto tetrahedron = lux::Dodecahedron(5.0f);
    {
        auto entity = registry.create();
        registry.emplace<lux::Mesh>(entity, tetrahedron);
        registry.emplace<glm::mat4>(entity, tetrahedronTransform);
    }

    for (size_t n = 0;n < 20;n++)
    {
        float angle = lux::PI2f / 20 * n;

        auto model3 = glm::scale(glm::mat4(1.0), glm::vec3(1.0));
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

        auto model4 = glm::scale(glm::mat4(1.0), glm::vec3(0.25));
        model4 = lux::MatrixUtils::RotateAroundAxis(model4, axis, glm::vec3(0.0, angle, 0.0));
        model4 = glm::translate(model4, glm::vec3(11.5f, 0, 0)); // radius             

        auto s_cube = lux::Cuboid(glm::vec3(-0.5), glm::vec3(0.5), glm::mat4(1));

        auto entity = registry.create();
        registry.emplace<lux::Mesh>(entity, s_cube);
        registry.emplace<glm::mat4>(entity, model4); // *sub_cube2->Transformation());
    }

    for (size_t n = 0;n < 180;n++)
    {
        const float length = lux::random(8.0f);
        const float angle = lux::random(lux::RADIANSf);

        auto rm4 = lux::MatrixUtils::RotateAroundAxis(glm::mat4(1.0), axis, glm::vec3(0.0, angle, 0.0));
        rm4 = glm::translate(rm4, glm::vec3(4, 0, 0)); // radius

        const float radius = 0.05f + lux::random(0.1f);

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

    auto lightCube = lux::Cuboid(glm::vec3(1.0f, 1.0f, 1.0f));

    const auto ground = lux::Lines::Grid(5000.f, 1000.f, 500.f, 100.f, -10.0f);
    const auto plane = lux::Plane(5000.f, 1000.f, 500.f, 100.f, -10.025f);
    
    

    
   
  


    // std::function<int(int,int,int,int)> valueFunction = [](int s, int d, int ct, int tt) { return -1; };
    // You could simply add a + before the first lambda : auto valueFunction = +[](...) {...};.
    // It would convert the lambda to a function pointer, so auto would be deduced as a function pointer.– HolyBlackCat
    // std::function<void(int, int)> callback = [&](int width, int height) -> void {}
    //std::function<void(int, int)> callback = [&](int width, int height) {};

    window.AddResizeListener([&](int width, int height)
    {
        camera->SetViewportSize(width, height);
        canvas.Resize(width, height);
        lcarsLayer.Resize(width, height);
    });
  
    lux::s_Data.SceneData.Projection = camera->GetProjection();
    lux::s_Data.SceneData.ViewProjection = camera->GetViewProjection();
    lux::s_Data.SceneData.View = camera->GetView();
    lux::s_Data.SceneData.Position = camera->GetPosition();
    lux::s_Data.SceneData.LookAt = camera->GetLookAt();
    lux::s_Data.SceneData.zFar = camera->GetZFar();
    lux::s_Data.SceneData.zNear = camera->GetZNear();

    lux::s_Data.SceneData.Shadows = true;
    lux::s_Data.SceneData.ShadowBias = shadow_bias;
    lux::s_Data.SceneData.ShadowSamples = shadow_samples;

    lux::s_Data.SceneData.LightPosition = lightPos;
    lux::s_Data.SceneData.LightColor = lux::Colors::DeepPink;
    
    auto sso = lux::ShaderStorageBuffer("SceneSpecification", 10, sizeof(lux::s_Data.SceneData), &lux::s_Data.SceneData);
    
    bool doShadows = lux::s_Data.SceneData.Shadows;

    while (!window.ShouldClose())
    {
        auto model = lux::MatrixUtils::Transform(
            lux::MatrixUtils::Transform(glm::mat4(1.0f), glm::vec3(scale, scale, scale), translate, mRotate)
            , glm::vec3(scale2, scale2, scale2), translate2, rotate2);
        //auto mvp = camera->GetViewProjection() * model;

        imguiLayer.BeginFrame();

        layers.RenderImGui();
       

        // 1. Render everything that casts a shadow.
        if (doShadows) shadows.RenderTextureCubemap(model, lightPos);

        // 2. render scene as normal
        canvas.Bind();
        lux::Renderer::Clear();
        lux::Renderer::SetViewport(0, 0, window.GetWidth(), window.GetHeight());

        // TODO, Merge setting a uniform texture and creating the texture.
        if (doShadows)
        {
            shadows.BindTextureCubemap(1);
            shader->SetUniform1i("depthMap", 1);
        }

        shader->SetUniformVec4f("u_Color", gray);
        auto entityView = registry.view<lux::Mesh, glm::mat4>();
        for (auto entity : entityView)
        {
            auto& mesh = entityView.get<lux::Mesh>(entity);
            auto& transformation = entityView.get<glm::mat4>(entity);
            shader->SetUniformMat4f("u_Model", model * transformation);
            mesh.Draw(shader);             
        }
        shader->SetUniformVec4f("u_Color", lux::Colors::NeonBlue);
        shader->SetUniformMat4f("u_Model", identity);
        plane.Draw(shader);

        shaderBase->SetUniformMat4f("u_Model", glm::translate(identity, lightPos));
        shaderBase->SetUniformVec4f("u_Color", lux::Colors::White);
        shaderBase->SetUniformVec4f("u_BrightColor", lux::Colors::BrightWhite);
        lightCube.Draw(shaderBase);

        shaderBase->SetUniformMat4f("u_Model", identity);
        shaderBase->SetUniformVec4f("u_Color", brightPinkColor); //lux::Colors::DeepPink);
        shaderBase->SetUniformVec4f("u_BrightColor", brightPinkColor);
        ground.Draw(shaderBase);

        // 130 (65 * 2) = distance, 4 = seconds        
        lightPos.x = lux::Renderer::GetTimestep().PingPong(lightPos.x, lightInc.x, -65.0f, 65.0f, 165.0f, 4.0f);       
        lux::s_Data.SceneData.LightPosition = lightPos;
        sso.Publish(&lux::s_Data.SceneData);

        layers.OnUpdate();

        ImGui::Begin("Parameters");
        ImGui::ColorEdit3("Glow Color", (float*)&brightPinkColor, imgui_misc_flags);
        ImGui::End();

        imguiLayer.EndFrame();

        canvas.Unbind();

        lux::Renderer::Clear();

        canvas.Draw();

        window.SwapBuffers();

        lux::Renderer::GetTimestep().Update();
    }
    window.Destroy();

    glfwTerminate();

    return 0;
}