#pragma once

//#include <glad/glad.h>
//VERY IMPORTANT : In preferences : C / C++->Preprocessor->Preprocessor Definitions(add) GLEW_STATIC in the; delimited list!
#define IMGUI_IMPL_OPENGL_LOADER_GLEW
#include <gl/glew.h>                       // Cross platform link between your graphics card and OpenGL.
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <chrono>
#include <future>
#include <thread>
#include <ctime>
#include <vector>
#include <algorithm>
#include <memory>
#include <numbers>
#include <typeinfo>
#include <map>

#include <stdio.h>

//#define SPDLOG_HEADER_ONLY
//#define SPDLOG_COMPILED_LIB
#include <spdlog/spdlog.h>

#define GLM_FORCE_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/normal.hpp>
#include <glm/gtx/string_cast.hpp>

//#ifndef STB_IMAGE_IMPLEMENTATION
//#define STB_IMAGE_IMPLEMENTATION
//#include <stb_image.h>
//#endif

#include <pqxx/pqxx>

#include <entt/entt.hpp>

#include <irrKlang/irrKlang.h>

#include <nlohmann/json.hpp>

#include <windows.h>

#include <OBJ_Loader.h>