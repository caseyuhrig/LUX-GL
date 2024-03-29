#shader vertex
#version 450 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;

out vec3 FragPos;
out vec3 Normal;

layout(std430, binding = 10) buffer CameraSpec {
    mat4 Projection;
    mat4 View;
    mat4 ViewProjection;
    vec3 Position;
    vec3 LookAt;
    float zFar;
    float zNear;
} camera;


uniform mat4 u_Model;


void main()
{
    FragPos = vec3(camera.View * u_Model * vec4(a_position, 1.0));
    Normal = vec3(camera.View * u_Model * vec4(a_normal, 0.0));
    gl_Position = camera.Projection * vec4(FragPos, 1.0);
};


#shader fragment
#version 450 core

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 BrightColor;


uniform vec4 u_Color;
uniform vec4 u_BrightColor;

void main()
{
    FragColor = u_Color;
    BrightColor = u_BrightColor;
};