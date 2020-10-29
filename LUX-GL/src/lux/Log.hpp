#pragma once

#include <iostream>
#include <string>
#define FMT_EXPORT
#define FMT_HEADER_ONLY
#include "fmt/format.h"
#include "fmt/printf.h"
#include <fmt/color.h>

#include "Time.hpp"


namespace lux {

	class Log
	{
	public:
		Log() {}

		template <typename S, typename... Args>
		static void LOG_DEBUG(const S& format_str, const Args&... args)
		{
			auto now = lux::Time::now();
			std::string message = fmt::sprintf(format_str, args...);
			fmt::printf("%02d:%02d:%02d [DEBUG] %s\n", now.Hour, now.Minute, now.Second, message);
		}

		template <typename S, typename... Args>
		static void LOG_INFO(const S& format_str, const Args&... args)
		{
			auto now = lux::Time::now();
			std::string message = fmt::sprintf(format_str, args...);		
			std::string level = fmt::format(fmt::fg(fmt::color::blue_violet) | fmt::emphasis::bold, "INFO");
			fmt::printf("%02d:%02d:%02d  [%s] %s\n", now.Hour, now.Minute, now.Second, level, message);
		}


		template <typename S, typename... Args>
		static void LOG_ERROR(const S& format_str, const Args&... args)
		{
			auto now = lux::Time::now();
			std::string message = fmt::sprintf(format_str, args...);
			std::string level = fmt::format(fmt::bg(fmt::color::black) | fmt::fg(fmt::color::red) | fmt::emphasis::bold, "ERROR");
			fmt::printf("%02d:%02d:%02d [%s] %s\n", now.Hour, now.Minute, now.Second, level, message);
		}


		template <typename S, typename... Args>
		static void LOG_FATAL(const S& format_str, const Args&... args)
		{
			auto now = lux::Time::now();
			std::string message = fmt::sprintf(format_str, args...);
			std::string level = fmt::format(fmt::bg(fmt::color::red) | fmt::fg(fmt::color::black) | fmt::emphasis::bold, "FATAL");
			fmt::printf("%02d:%02d:%02d [%s] %s\n", now.Hour, now.Minute, now.Second, level, message);
		}

	};

}

#define UX_LOG_DEBUG(...) lux::Log::LOG_DEBUG(__VA_ARGS__)
#define UX_LOG_INFO(...) lux::Log::LOG_INFO(__VA_ARGS__)
#define UX_LOG_ERROR(...) lux::Log::LOG_ERROR(__VA_ARGS__)
#define UX_LOG_FATAL(...) lux::Log::LOG_FATAL(__VA_ARGS__)


// ---------------------------------------------
// DUMPING GROUND FOR RANDOM NOTES ...it's the log right! ;)
// ---------------------------------------------

// C/C++  Optimizations /O2 /GL
// Linker Optimizations /LYCG

// for python
// As another option you could enable support of long file names in Windows 10 by changing 
// HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\FileSystem\LongPathsEnabled registry value.

// set ENV var for 64 bit default package install vcpkg

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
/*
void window1()
{

    auto ui = lux::Interface("LUX/GL (Monitor-Left)", lux::Interface::Position::LEFT);

    while (!ui.ShouldClose())
    {
        ui.Clear();

        ui.Draw();

        ui.SwapBuffers();
    }

    ui.Destroy();
}
*/


/*

if (lux::Monitors::GetCount() < 3)
{
    UX_FATAL_ERROR("3 Monitors are required for this program.");
    exit(1);
}

//auto window1 = lux::Window::Create(lux::Monitors::GetLeft());

class Interface1 : public Interface
{
public:
    Interface1() : Interface(lux::Window::Create(lux::Monitors::GetLeft()))
    {

    }
    void Render() const
    {
    }
}

auto system = lux::System::Create();
system->AddInterface();

system->Run();

*/