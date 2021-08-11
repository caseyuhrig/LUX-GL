#include "pch.hpp"

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
#include "lux/Scene/Entity.hpp"
#include "lux/Scene/Model.hpp"

#include "lux/Platform/Microsoft/Windows.hpp"
#include "lux/Interface.hpp"


// OpenEXR
// https://github.com/AcademySoftwareFoundation/openexr

// Light Scattering
// https://fabiensanglard.net/lightScattering/
// https://www.youtube.com/watch?v=s60Drbt3x0E
// http://learnwebgl.brown37.net/09_lights/lights_attenuation.html

// Depth of Field
// https://community.khronos.org/t/fast-depth-of-field-in-opengl-how-to-do-it/44300
// http://learnwebgl.brown37.net/

// Lightning Bolt
// https://gamedev.stackexchange.com/questions/71397/how-can-i-generate-a-lightning-bolt-effect

// Atmospheric Scattering
// https://developer.nvidia.com/gpugems/gpugems2/part-ii-shading-lighting-and-shadows/chapter-16-accurate-atmospheric-scattering

// Glitch Effect
// https://codemyui.com/image-glitch-effect-using-opengl-shading-language/

int main(int argc, char** argv)
{
    std::string name = "Casey";
    spdlog::info("hello {}", name);

    auto window = lux::Window("LUX/GL", 1500, 900);
    window.Center();
    window.FillWorkArea();

    lux::Renderer::Init(window.GetWidth(), window.GetHeight());
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
    

   

    auto gray = glm::vec4(0.45f, 0.45f, 0.45f, 1.0f);
    
    const auto axis = glm::vec3(0.0f, 0.0f, 0.0f);

    auto brightPinkColor = lux::Colors::DeepPink * 2.0f;
    const ImGuiColorEditFlags imgui_misc_flags = ImGuiColorEditFlags_Float;




    auto& shader = lux::Renderer::ShaderLibrary().Get("PointShadows");
    shader->SetUniform1i("u_ReverseNormals", false);
    //shader->SetUniform1i("shadows", useShadows);
    //shader->SetUniformVec4f("u_LightColor", lux::Colors::NeonPink);

    auto& shaderBase = lux::Renderer::ShaderLibrary().Get("Base");
    shaderBase->SetUniformVec4f("u_Color", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    shaderBase->SetUniformVec4f("u_BrightColor", glm::vec4(1.5f, 1.5f, 1.5f, 1.0f));




    auto defaultMaterial = lux::Material(shaderBase);




  


    auto canvas = lux::Canvas(window.GetWidth(), window.GetHeight(), 8);
    
    auto camera = lux::Camera::Create(window.GetWidth(), window.GetHeight(), glm::vec3(0.0f, 0.0f, 50.0f), glm::vec3(0.0f, 0.0f, 0.0f));

    auto scene = lux::Scene::Create(camera);
    scene->Publish();

    auto shadows = lux::Shadows(scene);



/*
    auto imguiLayer = lux::ImGuiLayer(&window);
    imguiLayer.SetCamera(camera);
    imguiLayer.SetMaterial(&defaultMaterial);
    imguiLayer.SetLights(&lights);
    imguiLayer.SetShaderBase(shaderBase);
    imguiLayer.SetLightPos(&lightPos);
    imguiLayer.SetCubeTransforms(&scaleCube, &translateCube, &rotateCube);
    imguiLayer.Set1(&scale, &translate, &mRotate);
    imguiLayer.Set2(&scale2, &translate2, &rotate2);
    */


    lux::LcarsLayer lcarsLayer = lux::LcarsLayer(window.GetWidth(), window.GetHeight(), mRotate);
    lcarsLayer.SetLights(lights);
    lcarsLayer.SetRotate(mRotate);

    lux::LayerStack layers = lux::LayerStack();  
    //layers.Attach(&imguiLayer);
    layers.Attach(&lcarsLayer);
    

    auto& registry = scene->GetRegistry();
    
    auto segment = lux::Segment::Create(0.5f, 1.0f, 0.0f, 45.0f, 2.0f);
    auto index = 0;
    for (auto n = 0.0f; n < 4.0f; n++)
    {
        auto angle = lux::RAD<float> / 4.0f * n;
        auto transform = glm::rotate(glm::identity<glm::mat4>(), angle, glm::vec3(0.0, 1.0, 0.0));
        lux::Entity::MakeMesh(registry, scene, "Seg1n" + index, segment, transform);
        index++;
    }

    index = 0;
    auto segment2 = lux::Segment::Create(1.25f, 1.5f, 0.0f, 15.0f, 1.0f);
    for (auto n = 0.0f; n < 18.0f; n++)
    {
        auto angle = lux::RAD<float> / 18.0f * n;
        auto transform = glm::rotate(glm::identity<glm::mat4>(), angle, glm::vec3(0.0f, 1.0f, 0.0f));
        lux::Entity::MakeMesh(registry, scene, "Seg2n" + index, segment2, transform);
        index++;
    }
    
    
    auto dodecahedron = lux::Dodecahedron::Create(5.0f);
    lux::Entity::MakeMesh(registry, scene, "Dodecahedron", dodecahedron, glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 7.0f, 0.0f)));

    for (auto n = 0; n < 20; n++)
    {       
        auto angle = lux::RAD<float> / 20.0f * n;
        auto model3 = glm::scale(glm::identity<glm::mat4>(), glm::vec3(1.0f));
        model3 = lux::MatrixUtils::RotateAroundAxis(model3, axis, glm::vec3(0.0f, angle, 0.0f));
        model3 = glm::translate(model3, glm::vec3(10.0f, 0.0f, 0.0f)); // radius   
        
        auto cube = lux::Cuboid::Create(glm::vec3(-0.5f), glm::vec3(0.5f), glm::identity<glm::mat4>());

        lux::Entity::MakeMesh(registry, scene, "Cubes 1" + n, cube, model3);
    }
    

    for (auto n = 0;n < 60;n++)
    {
        auto angle = lux::RAD<float> / 60.0f * n;
        auto model4 = glm::scale(glm::identity<glm::mat4>(), glm::vec3(0.25));
        model4 = lux::MatrixUtils::RotateAroundAxis(model4, axis, glm::vec3(0.0, angle, 0.0));
        model4 = glm::translate(model4, glm::vec3(11.5f, 0.0f, 0.0f)); // radius             

        auto s_cube = lux::Cuboid::Create(glm::vec3(-0.5), glm::vec3(0.5), glm::identity<glm::mat4>());

        lux::Entity::MakeMesh(registry, scene, "Cubes 2" + n, s_cube, model4);
    }

    for (size_t n = 0;n < 180;n++)
    {
        auto length = lux::random(8.0f);
        auto angle = lux::random(lux::RAD<float>);
        auto rm4 = lux::MatrixUtils::RotateAroundAxis(glm::mat4(1.0f), axis, glm::vec3(0.0f, angle, 0.0f));
        rm4 = glm::translate(rm4, glm::vec3(4.0f, 0.0f, 0.0f)); // radius
        auto radius = 0.05f + lux::random(0.1f);

        auto s_cube = lux::Cuboid::Create(glm::vec3(0.0f, -radius, -radius), glm::vec3(0.5f + length, radius, radius), glm::mat4(1.0f));

        lux::Entity::MakeMesh(registry, scene, "Cubes 3" + n, s_cube, rm4);
    }

    for (size_t n = 0;n < 300;n++)
    {
        auto length = lux::random(5.0f);
        auto angle = lux::random(lux::RADIANSf);

        glm::mat4 rm4 = lux::MatrixUtils::RotateAroundAxis(glm::mat4(1.0), axis, glm::vec3(0.0, angle, 0.0));
        rm4 = rm4 * glm::translate(rm4, glm::vec3(8, 0, 0)); // radius

        auto radius = 0.025f + lux::random(0.1f);

        auto moreCube = lux::Cuboid::Create(glm::vec3(0.5f, -radius, -radius), glm::vec3(0.5f + length, radius, radius), glm::mat4(1.0f));

        lux::Entity::MakeMesh(registry, scene, "Cubes 4" + n, moreCube, rm4);
    }

    // the angled ones
    for (size_t n = 0;n < 100;n++)
    {
        auto length = lux::random(5.0f);
        auto angle = lux::random(lux::RADIANSf);
        auto radius = lux::random(0.1f);

        auto bar = lux::Cuboid::Create(glm::vec3(0.0, -radius, -radius), glm::vec3(0.5 + length, radius, radius), glm::mat4(1.0f));

        glm::mat4 rm4 = lux::MatrixUtils::RotateAroundAxis(glm::mat4(1.0), axis, glm::vec3(0.0, angle, 0.2));
        rm4 = glm::translate(rm4, glm::vec3(4, 0, 0));

        lux::Entity::MakeMesh(registry, scene, "Cubes 5" + n, bar, rm4);
    }

    auto lightCube = lux::Cuboid(glm::vec3(1.0f, 1.0f, 1.0f));

    const auto ground = lux::Lines::Grid(5000.f, 1000.f, 500.f, 100.f, -10.0f);
    const auto plane = lux::Plane(5000.f, 1000.f, 500.f, 100.f, -10.025f);
    
    // ground cubes
    for (size_t n = 0;n < 1000;n++)
    {
        auto length = lux::random(1.0f);
        auto radius = 40.0f;
        auto x = lux::random(-radius) + lux::random(radius);
        auto z = lux::random(-radius) + lux::random(radius);

        auto cube = lux::Cuboid::Create(glm::vec3(-length), glm::vec3(length), glm::mat4(1.0f));
        auto transform = glm::translate(glm::identity<glm::mat4>(), glm::vec3(x, -10.0f, z));

        lux::Entity::MakeMesh(registry, scene, "Ground Cube", cube, transform);
    }

    lux::Lines lines;
    for (int n = 0;n < 1000;n++)
    {
        auto radius = 40.0f;
        auto x1 = lux::random(-radius) + lux::random(radius);
        auto y1 = lux::random(-radius) + lux::random(radius);
        auto z1 = lux::random(-radius) + lux::random(radius);
        auto x2 = lux::random(-radius) + lux::random(radius);
        auto y2 = lux::random(-radius) + lux::random(radius);
        auto z2 = lux::random(-radius) + lux::random(radius);

        lines.Add(glm::vec3(x1,y1,z1), glm::vec3(x2,y2,z2));
    }
    lines.Build();
    



    lux::s_Data.CameraData.Projection = camera->GetProjection();
    lux::s_Data.CameraData.ViewProjection = camera->GetViewProjection();
    lux::s_Data.CameraData.View = camera->GetView();
    lux::s_Data.CameraData.Position = camera->GetPosition();
    lux::s_Data.CameraData.LookAt = camera->GetLookAt();
    //lux::s_Data.CameraData.zFar = camera->GetZFar();
    //lux::s_Data.CameraData.zNear = camera->GetZNear();
    lux::s_Data.CameraData.ViewportWidth = window.GetWidth();
    lux::s_Data.CameraData.ViewportHeight = window.GetHeight();
    lux::s_Data.CameraData.AspectRatio = static_cast<float>(lux::s_Data.CameraData.ViewportWidth) / static_cast<float>(lux::s_Data.CameraData.ViewportHeight);
    lux::s_Data.CameraData.Projection = glm::perspective(glm::radians(lux::s_Data.CameraData.Angle), lux::s_Data.CameraData.AspectRatio, lux::s_Data.CameraData.zNear, lux::s_Data.CameraData.zFar);
    lux::s_Data.CameraData.View = glm::lookAt(lux::s_Data.CameraData.Position, lux::s_Data.CameraData.LookAt, lux::s_Data.CameraData.Up);
    lux::s_Data.CameraData.ViewProjection = lux::s_Data.CameraData.Projection * lux::s_Data.CameraData.View;

    lux::s_Data.ShadowData.Shadows = true;
    lux::s_Data.ShadowData.ShadowBias = 0.3f;
    lux::s_Data.ShadowData.ShadowSamples = 10;
    lux::s_Data.ShadowData.ShadowAmount = 1.0f;

    lux::s_Data.LightData.LightPosition = lightPos;
    lux::s_Data.LightData.LightColor = lux::Colors::DeepPink;

    lux::s_Data.MaterialData.Color = gray;


//    for (auto n = 0;n<10;n++)
//        lux::s_Data.TransformsData.Transform[0] = glm::mat4(1.0f);

    //lux::s_Data.TransformsData.Transform[0] = glm::mat4(1.0f); 
    
    
    auto cameraSpec = lux::ShaderStorageBuffer("CameraSpec", 10, 272, &lux::s_Data.CameraData);
    auto shadowSpec = lux::ShaderStorageBuffer("ShadowSpec", 11, 16, &lux::s_Data.ShadowData);
    auto lightSpec = lux::ShaderStorageBuffer("LightSpec", 12, 32, &lux::s_Data.LightData);
    auto materialSpec = lux::ShaderStorageBuffer("MaterialSpec", 13, 80, &lux::s_Data.MaterialData);
    //auto transformsSpec = lux::ShaderStorageBuffer("TransformsSpec", 13, 640, &lux::s_Data.TransformsData);
    
    //imguiLayer.SetShadowSamples(&lux::s_Data.ShadowData.ShadowSamples);
    //imguiLayer.SetShadowBias(&lux::s_Data.ShadowData.ShadowBias);

    //bool& doShadows = lux::s_Data.ShadowData.Shadows;

    /*
    spdlog::trace("   INT SIZE: %d", sizeof(int));
    UX_LOG_DEBUG(" FLOAT SIZE: %d", sizeof(float));
    UX_LOG_DEBUG("  BOOL SIZE: %d", sizeof(bool));
    UX_LOG_DEBUG("  mat4 SIZE: %d", sizeof(glm::mat4));
    UX_LOG_DEBUG("  vec3 SIZE: %d", sizeof(glm::vec3));
    UX_LOG_DEBUG("  vec4 SIZE: %d", sizeof(glm::vec4));
    UX_LOG_DEBUG("camera SIZE: %d", sizeof(lux::RendererData::CameraInfo));
    UX_LOG_DEBUG("shadow SIZE: %d", sizeof(lux::RendererData::ShadowInfo));
    UX_LOG_DEBUG(" light SIZE: %d", sizeof(lux::RendererData::LightInfo));
    */

    // std::function<int(int,int,int,int)> valueFunction = [](int s, int d, int ct, int tt) { return -1; };
    // You could simply add a + before the first lambda : auto valueFunction = +[](...) {...};.
    // It would convert the lambda to a function pointer, so auto would be deduced as a function pointer.– HolyBlackCat
    // std::function<void(int, int)> callback = [&](int width, int height) -> void {}
    // std::function<void(int, int)> callback = [&](int width, int height) {};

    window.AddResizeListener([&](int width, int height)
        {
            camera->SetViewportSize(width, height);
            lux::s_Data.CameraData.Resize(width, height);
            cameraSpec.Publish(&lux::s_Data.CameraData);

            canvas.Resize(width, height);
            lcarsLayer.Resize(width, height);
        });

    

    auto lightShader = shaderBase->Duplicate();
    lightShader->SetUniformMat4f("u_Model", glm::translate(glm::identity<glm::mat4>(), lightPos));
    lightShader->SetUniformVec4f("u_Color", lux::Colors::White);
    lightShader->SetUniformVec4f("u_BrightColor", lux::Colors::BrightWhite);

    auto planeShader = shader->Duplicate();
    planeShader->SetUniformMat4f("u_Model", glm::identity<glm::mat4>());
    planeShader->SetUniformVec4f("u_Color", lux::Colors::NeonBlue);

    auto groundShader = shaderBase->Duplicate();
    groundShader->SetUniformMat4f("u_Model", glm::identity<glm::mat4>());
    //groundShader->SetUniformVec4f("u_Color", brightPinkColor);
    //groundShader->SetUniformVec4f("u_BrightColor", brightPinkColor * 2.0f);
    groundShader->SetUniformVec4f("u_Color", lux::Colors::Black);
    groundShader->SetUniformVec4f("u_BrightColor", lux::Colors::Black);

    shader->SetUniformVec4f("u_Color", gray);

    //auto robotMesh = lux::Mesh();
    //robotMesh.Load("head_Cube", "res/meshes/Toon-Female-blockout.obj");

    //std::string soundBackgroundFilename = "res/audio/tlsf/engineloop.wav";

    std::string soundFilename = "res/audio/tlsf/Laser1.wav";

    irrklang::ISoundEngine* engine = irrklang::createIrrKlangDevice();

    //std::cout << "CAMERA LOCATION: " << glm::to_string(camera_position) << std::endl;
    //std::cout << " CAMERA LOOK AT: " << glm::to_string(camera_look_at) << std::endl;

    auto sound_camera_location = irrklang::vec3df(lux::s_Data.CameraData.Position.x, lux::s_Data.CameraData.Position.y, lux::s_Data.CameraData.Position.z);
    auto sound_camera_look_at = irrklang::vec3df(lux::s_Data.CameraData.LookAt.x, lux::s_Data.CameraData.LookAt.y, lux::s_Data.CameraData.LookAt.z);

    engine->setListenerPosition(sound_camera_location, sound_camera_look_at);
    engine->setSoundVolume(1.0f);
    engine->setDefault3DSoundMinDistance(10.0f); // [default] 1.0f

    //engine->play2D(soundBackgroundFilename.c_str(), true); // played looped

    //engine->play2D("res/audio/sound_effect_3.wav");
    //auto soundPos = irrklang::vec3df(lightPos.x, lightPos.y, lightPos.z);
    //irrklang::ISound* sound1 = engine->play3D(soundFilename.c_str(), soundPos, true, false, true); // play looped, start paused, track
    //irrklang::ISound* sound1 = engine->play3D(soundBackgroundFilename.c_str(), soundPos, true, false, true); // play looped, start paused, track
    //irrklang::ISound* sound1 = NULL;
    //sound1->setMinDistance(5.0f);
    //sound1->

    //sound1->setPlayPosition(0);
    //sound1->
    //sound1->
    //engine->play3D(soundFilename.c_str(), soundPos);

    auto model = glm::identity<glm::mat4>();

    spdlog::info("1 -------------------------------------------");

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    std::string glsl_version = "#version 450";
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window.GetGlfwWindow(), true);
    ImGui_ImplOpenGL3_Init(glsl_version.c_str());
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    //auto io = ImGui::GetIO();

    spdlog::info("2 -------------------------------------------");

    //auto str = glm::to_string(gray);
    //UX_LOG_DEBUG("COLOR: %s", str.c_str());
    //float dot = glm::dot(gray.rgb(), glm::vec3(0.2126, 0.7152, 0.0722));
    //UX_LOG_DEBUG("DOT: %f", dot);

    //auto girlModel = lux::Model::Create("res/meshes/Toon-Female-blockout.obj");
    //auto girlModel = lux::Model::Create("c:/Users/casey/Downloads/race_spaceship.obj");
    //lux::Entity::MakeModel(registry, scene, "Girl", girlModel, glm::identity<glm::mat4>());

    long long n = 0L;

    while (!window.ShouldClose())
    {
        try {
            //spdlog::info("COUNT: {}", n);

            // 1. Render everything that casts a shadow.
            if (lux::s_Data.ShadowData.Shadows)
            {
                shadows.RenderTextureCubemap(model, lightPos);
                // Bind resulting cubemap to shader.
                shadows.BindTextureCubemap(1);
                shader->SetUniform1i("depthMap", 1);
            }

            // 2. render scene as normal
            canvas.Bind();
            lux::Renderer::Clear();
            lux::Renderer::SetViewport(0, 0, window.GetWidth(), window.GetHeight());
            // TODO, Merge setting a uniform texture and creating the texture.
            //if (lux::s_Data.ShadowData.Shadows)
            //{
                //shadows.BindTextureCubemap(1);
                //shader->SetUniform1i("depthMap", 1);
            //}


            const auto& entityView = registry.view<lux::MeshComponent, lux::TransformComponent, lux::TagComponent>();
            for (auto entityHandle : entityView)
            {
                const auto& [mesh, transform, tag] = entityView.get<lux::MeshComponent, lux::TransformComponent, lux::TagComponent>(entityHandle);
                if (tag != "Ground Cube")
                {
                    shader->SetUniformMat4f("u_Model", model * transform.Transform);
                    lux::Renderer::Draw(mesh, shader);
                }
            }

            const auto& entityView2 = registry.view<lux::ModelComponent, lux::TransformComponent, lux::TagComponent>();
            for (auto entityHandle : entityView2)
            {
                const auto& [mod, transform, tag] = entityView2.get<lux::ModelComponent, lux::TransformComponent, lux::TagComponent>(entityHandle);
                //model->Draw(shader);
                //lux::Renderer::Draw(model, shader);
                shader->SetUniformMat4f("u_Model", model * transform.Transform);
                mod.Model->Draw(shader);
            }

            //shader->SetUniformMat4f("u_Model", model);
            //girlModel->Draw(shader);

            //shader->SetUniformMat4f("u_Model", model);
            //robotMesh.Draw(shader);

            lux::Renderer::Draw(lightCube, lightShader);
            planeShader->SetUniformMat4f("u_Model", glm::identity<glm::mat4>());
            lux::Renderer::Draw(plane, planeShader);
            ground.Draw(groundShader);

            //lux::Renderer::Draw(ground, groundShader);

            //lines.Draw(groundShader);


            const auto& groundView = registry.view<lux::MeshComponent, lux::TransformComponent, lux::TagComponent>();
            for (auto entityHandle : groundView)
            {
                const auto& [mesh, transform, tag] = groundView.get<lux::MeshComponent, lux::TransformComponent, lux::TagComponent>(entityHandle);
                if (tag == "Ground Cube")
                {
                    planeShader->SetUniformMat4f("u_Model", glm::identity<glm::mat4>() * transform.Transform);
                    lux::Renderer::Draw(mesh, planeShader);
                }
            }


            // 130 (65 * 2) = distance, 4 = seconds        
            //lightPos.x = lux::Renderer::GetTimestep().PingPong(lightPos.x, lightInc.x, -65.0f, 65.0f, 165.0f, 4.0f);      
            //lightShader->SetUniformMat4f("u_Model", glm::translate(glm::identity<glm::mat4>(), lightPos));
            //lux::s_Data.LightData.LightPosition = lightPos;
            //lightSpec.Publish(&lux::s_Data.LightData);

            lcarsLayer.OnUpdate();

            bool changed = false;


            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            ImGui::Begin("Parameters");
            
            ImGui::Text("Transformation");
            ImGui::SliderFloat("M1 Scale", &scale, 1.0f, 50.0f);
            if (ImGui::IsItemActive()) changed = true;
            ImGui::SliderFloat3("M1 Translate", &translate[0], -20.0f, 20.0f);
            if (ImGui::IsItemActive()) changed = true;
            ImGui::SliderFloat3("M1 Rotate", &mRotate[0], 0.0f, lux::RAD<float>);
            if (ImGui::IsItemActive()) changed = true;
            ImGui::Separator();
            ImGui::SliderFloat("M2 Scale", &scale2, 1.0f, 50.0f);
            if (ImGui::IsItemActive()) changed = true;
            ImGui::SliderFloat3("M2 Translate", &translate2[0], -20.0f, 20.0f);
            if (ImGui::IsItemActive()) changed = true;
            ImGui::SliderFloat3("M2 Rotate", &rotate2[0], 0.0f, lux::RAD<float>);
            if (ImGui::IsItemActive()) changed = true;

            ImGui::Separator();
            ImGui::Text("Material");
            ImGui::ColorEdit3("Color", &lux::s_Data.MaterialData.Color[0], imgui_misc_flags);
            if (ImGui::IsItemActive()) changed = true;
            ImGui::ColorEdit3("Ambient", &lux::s_Data.MaterialData.Ambient[0], imgui_misc_flags);
            if (ImGui::IsItemActive()) changed = true;
            ImGui::ColorEdit3("Diffuse", &lux::s_Data.MaterialData.Diffuse[0], imgui_misc_flags);
            if (ImGui::IsItemActive()) changed = true;
            ImGui::ColorEdit3("Specular", &lux::s_Data.MaterialData.Specular[0], imgui_misc_flags);
            if (ImGui::IsItemActive()) changed = true;
            ImGui::SliderFloat("Shininess", &lux::s_Data.MaterialData.Shininess, 0.0f, 128.0f);
            if (ImGui::IsItemActive()) changed = true;

            //Ambient = glm::vec4(glm::vec3(0.3f), 1.0f); //vec3(1.0f, 0.5f, 0.31f); // 16  16
            //glm::vec4 Diffuse = glm::vec4(1.0f); //vec3(1.0f, 0.5f, 0.31f);           // 16  32
            //glm::vec4 Specular = glm::vec4(1.0f); //vec3(0.5f, 0.5f, 0.5f);           // 16  48
            //float Shininess

            ImGui::Separator();
            ImGui::Text("Shadows");
            ImGui::SliderInt("Shadow Samples", &lux::s_Data.ShadowData.ShadowSamples, 1, 20);
            if (ImGui::IsItemActive()) changed = true;
            ImGui::SliderFloat("Shadow Bias", &lux::s_Data.ShadowData.ShadowBias, 0.0f, 1.0f);
            if (ImGui::IsItemActive()) changed = true;
            ImGui::SliderFloat("Shadow Alpha", &lux::s_Data.ShadowData.ShadowAmount, 0.0f, 1.0f);
            if (ImGui::IsItemActive()) changed = true;
            ImGui::Checkbox("Shadows", &lux::s_Data.ShadowData.Shadows);
            if (ImGui::IsItemActive()) changed = true;

            ImGui::Separator();
            ImGui::Text("Camera");
            ImGui::SliderFloat3("Camera Position", &lux::s_Data.CameraData.Position[0], -500.0f, 500.0f);
            if (ImGui::IsItemActive()) changed = true;
            ImGui::SliderFloat3("Camera Look At", (float*)&lux::s_Data.CameraData.LookAt, -100.0f, 100.0f);
            if (ImGui::IsItemActive()) changed = true;
            ImGui::SliderFloat("Angle", &lux::s_Data.CameraData.Angle, 0.0f, 90.0f);
            if (ImGui::IsItemActive()) changed = true;
            ImGui::SliderFloat("Z Near", &lux::s_Data.CameraData.zNear, 0.01f, 2000.0f);
            if (ImGui::IsItemActive()) changed = true;
            ImGui::SliderFloat("Z Far", &lux::s_Data.CameraData.zFar, 0.01f, 2000.0f);
            if (ImGui::IsItemActive()) changed = true;

            ImGui::Separator();
            ImGui::Text("Light Source");
            ImGui::SliderFloat3("Light Position", &lux::s_Data.LightData.LightPosition[0], -100.0f, 100.0f);
            if (ImGui::IsItemActive()) changed = true;
            ImGui::ColorEdit3("Light Color", &lux::s_Data.LightData.LightColor[0], imgui_misc_flags);
            if (ImGui::IsItemActive()) changed = true;
            ImGui::Text("Bloom");
            ImGui::ColorEdit3("Glow Color", &brightPinkColor[0], imgui_misc_flags);
            if (ImGui::IsItemActive()) {
                groundShader->SetUniformVec4f("u_Color", brightPinkColor);
                groundShader->SetUniformVec4f("u_BrightColor", brightPinkColor * 2.0f);
            }

            //ImGui::Checkbox("Skybox", (bool*)&useSkybox); ImGui::SameLine();
            //ImGui::Checkbox("Cubemap", (bool*)&useCubemap);

            //ImGui::Checkbox("Animate", (bool*)&animate); ImGui::SameLine();
            //ImGui::Checkbox("Show Text", (bool*)&showText);

            if (ImGui::Button("Play Sound"))
            {
                auto soundPos = irrklang::vec3df(lightPos.x, lightPos.y, lightPos.z);
                engine->play3D(soundFilename.c_str(), soundPos);
            }
            ImGui::SameLine();
            if (ImGui::Button("Supersize"))
            {
                window.SuperSize();
            }
            ImGui::SameLine();
            if (ImGui::Button("Resize"))
            {
                //window.Center();
                window.FillWorkArea();
            }

            if (ImGui::Button("Exit"))
            {
                std::cout << "EXIT" << std::endl;
                //break;
                //exit(0);
                window.Close();
            }
            
            ImGui::End();

            //ImGui::ShowDemoWindow();

            // Rendering
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            //imguiLayer.EndFrame();



            if (changed)
            {
                shader->SetUniformVec4f("u_Color", lux::s_Data.MaterialData.Color);

                materialSpec.Publish(&lux::s_Data.MaterialData);

                lux::s_Data.CameraData.Update();
                cameraSpec.Publish(&lux::s_Data.CameraData);

                lightSpec.Publish(&lux::s_Data.LightData);
                lightPos = lux::s_Data.LightData.LightPosition;
                //lightPos.z = -lightPos.z;
                lightShader->SetUniformMat4f("u_Model", glm::translate(glm::identity<glm::mat4>(), lightPos));

                shadowSpec.Publish(&lux::s_Data.ShadowData);

                model = lux::MatrixUtils::Transform(
                    lux::MatrixUtils::Transform(glm::mat4(1.0f), glm::vec3(scale, scale, scale), translate, mRotate)
                    , glm::vec3(scale2, scale2, scale2), translate2, rotate2);
            }


            canvas.Unbind();

            lux::Renderer::Clear();

            canvas.Draw();

            window.SwapBuffers();

            lux::Renderer::GetTimestep().Update();

            n++;
        }
        catch (const std::exception& exception) {
            spdlog::critical("MAIN LOOP: {}", exception.what());
        }
    }
    window.Destroy();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();

    return 0;
}