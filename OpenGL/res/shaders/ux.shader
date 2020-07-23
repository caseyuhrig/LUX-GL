#shader vertex
#version 440 core

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec4 in_color;
layout(location = 2) in vec2 in_texCoord;
layout(location = 3) in vec3 in_normal;

out vec4 v_Color;
out vec2 v_TexCoord;
out vec3 v_Normal;

uniform mat4 u_MVP; // model view projection matrix


void main()
{
    gl_Position = u_MVP * vec4(in_position, 1.0);
    v_Color = in_color;
    v_TexCoord = in_texCoord;
    v_Normal = in_normal;
};


#shader fragment
#version 440 core

layout(location = 0) out vec4 color;

in vec4 v_Color;
in vec2 v_TexCoord;
in vec3 v_Normal;

uniform vec4 u_Color;
//uniform sampler2D u_Texture;

void main()
{
    //vec4 texColor = texture(u_Texture, v_TexCoord);
    //color = vec4(1.0f, 1.0f, 1.0f, 1.0f); //texColor;
    color = v_Color;
    //color.a = 0.5;
};