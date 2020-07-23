#shader vertex
#version 440 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;

layout(std140, binding = 0) uniform Awesome
{
    vec4      ux_color;
// material
vec4      ux_ambient_color;
vec4      ux_diffuse_color;
vec4      ux_specular_color;
float     ux_specular_shininess;
// camera
vec4      ux_camera_position;
vec4      ux_camera_look_at;
// light
vec4      ux_light_pos;
vec4      ux_light_color;
vec4      ux_light_ambient;
vec4      ux_light_diffuse;
vec4      ux_light_specular;
// core matrix(s)
mat4      ux_view;
mat4      ux_proj;
// post-production
float     ux_gamma;
};

out vec3 FragPos;
out vec3 Normal;

//uniform mat4 u_MVP; // model view projection matrix
uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_proj;

// https://learnopengl.com/Lighting/Basic-Lighting
// normal help, calc normals prior to the GPU
// Normal = mat3(transpose(inverse(model))) * aNormal; 

void main()
{
    FragPos = vec3(ux_view * u_model * vec4(a_position, 1.0));
    // OMG very important!
    //Normal = vec3(u_view * u_model * vec4(a_normal, 0.0));
    //Normal = mat3(transpose(inverse(u_model))) * a_normal;
    Normal = mat3(transpose(inverse(u_model))) * a_normal;
    Normal = vec3(ux_view * vec4(Normal, 0.0));

    gl_Position = u_proj * vec4(FragPos, 1.0);


    //FragPos = vec3(u_model * vec4(a_position, 1.0));
    // OMG very important!
    //Normal = vec3(u_model * vec4(a_normal, 0.0));
    //gl_Position = u_proj * u_view * vec4(FragPos, 1.0);
};



#shader fragment
#version 440 core

//layout(location = 0) out vec4 color;
layout(location = 0) out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

//uniform vec4      u_Color;
//uniform sampler2D u_Texture;

// material
uniform vec3      u_ambient_color;
uniform vec3      u_diffuse_color;
uniform vec3      u_specular_color;
uniform float     u_specular_shininess;
// camera
//uniform vec3      u_camera_position;
//uniform vec3      u_camera_look_at;
                  
// light
uniform vec3      u_light_pos;
uniform vec3      u_light_color;
uniform vec3      u_light_ambient;
uniform vec3      u_light_diffuse;
uniform vec3      u_light_specular;

uniform mat4      u_view;
uniform mat4      u_proj;

uniform float     u_gamma;


layout(std140, binding = 1) uniform Pooper
{
    mat4      p_view;
    mat4      p_proj;
};


layout (std140, binding = 0) uniform Awesome
{
    vec4      ux_color;
    // material
    vec4      ux_ambient_color;
    vec4      ux_diffuse_color;
    vec4      ux_specular_color;
    float     ux_specular_shininess;
    // camera
    vec4      ux_camera_position;
    vec4      ux_camera_look_at;
    // light
    vec4      ux_light_pos;
    vec4      ux_light_color;
    vec4      ux_light_ambient;
    vec4      ux_light_diffuse;
    vec4      ux_light_specular;
    // core matrix(s)
    mat4      ux_view;
    mat4      ux_proj;
    // post-production
    float     ux_gamma;
};

layout(std140, binding = 2) uniform Super
{
    vec4 s_color1;
    vec4 s_color2;
};


float rand(vec2 co) {
    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}


void main()
{
        // ambient
        vec3 ambient = vec3(ux_light_ambient) * vec3(ux_ambient_color); // light_color
        //vec3 ambient = u_light_ambient * vec3(s_color1);
        //vec3 light_pos = vec3(u_view * vec4(ux_light_pos, 1.0)); // GOOD
        vec3 light_pos = vec3(u_view * ux_light_pos); // GOOD
        // diffuse 
        vec3 norm = normalize(Normal);
        vec3 lightDir = normalize(light_pos - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = vec3(ux_light_diffuse) * (diff * vec3(ux_diffuse_color)); //light_color;
        // specular
        //vec3 camera_pos = vec3(u_view * vec4(u_camera_position, 0.0)); // CAMERA
        vec3 camera_pos = vec3(u_view * ux_camera_position); // CAMERA
        vec3 viewDir = normalize(camera_pos - FragPos);
        vec3 reflectDir = reflect(-lightDir, viewDir);

        float spec = pow(max(dot(viewDir, reflectDir), 0.0), ux_specular_shininess);
        //vec3 specular = vec3(ux_light_specular) * (spec * (vec3(ux_specular_color))); //light_color;
        vec3 specular = vec3(ux_light_specular) * (spec * vec3(ux_specular_color)); //light_color;
        //vec3 specular = vec3(ux_specular_color) * spec * vec3(ux_light_specular);

        //u_Color[0] = 1.0f;

        //vec3 result = (ambient + diffuse + specular) * vec3(u_Color);
        //vec3 result = (ambient + diffuse + specular) * 1.1f;
        vec3 result = ambient + diffuse + specular;
        FragColor = vec4(result, 1.0);

        //float gamma = 2.2;
        FragColor.rgb = pow(FragColor.rgb, vec3(1.0 / ux_gamma));
};