#shader vertex
#version 440 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;

out vec3 FragPos;
out vec3 Normal;

//uniform mat4 u_MVP; // model view projection matrix
uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_proj;


void main()
{
    FragPos = vec3(u_view * u_model * vec4(a_position, 1.0));
    // OMG very important!
    Normal = vec3(u_view * u_model * vec4(a_normal, 0.0));
    gl_Position = u_proj * vec4(FragPos, 1.0);


    //FragPos = vec3(u_model * vec4(a_position, 1.0));
    // OMG very important!
    //Normal = vec3(u_model * vec4(a_normal, 0.0));
    //gl_Position = u_proj * u_view * vec4(FragPos, 1.0);
};


#shader fragment
#version 440 core

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 BrightColor;

void main()
{
    FragColor = vec4(1.0, 1.0, 1.0, 1.0);
    BrightColor = vec4(1.5, 1.5, 1.5, 1.0);
};