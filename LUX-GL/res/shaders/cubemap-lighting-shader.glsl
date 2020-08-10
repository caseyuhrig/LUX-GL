#shader vertex
#version 450 core
layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
layout(location = 2) in vec4 a_color;
layout(location = 3) in vec2 a_texCoord;
layout(location = 4) in int a_mat_id;

out vec3 FragPos;
out vec3 Normal;

struct Scene {
    mat4 model;
    mat4 view;
    mat4 proj;
    float gamma;
};

const unsigned int SCENES = 1;
const unsigned int ACTIVE_SCENE = 0;

layout(std140, binding = 6) uniform SceneProperties
{
    Scene scenes[SCENES];
};


uniform mat4 u_model;


void main()
{
    Scene scene = scenes[ACTIVE_SCENE];

    FragPos = vec3(scene.view * u_model * vec4(a_position, 1.0));
    // OMG very important! rotate the normals by the model matrix
    // I think might still need work.
    //Normal = vec3(u_view * u_model * vec4(a_normal, 0.0));
    //Normal = mat3(transpose(inverse(u_model))) * a_normal;
    Normal = mat3(transpose(inverse(u_model))) * a_normal;
    Normal = vec3(scene.view * vec4(Normal, 0.0));

    gl_Position = scene.proj * vec4(FragPos, 1.0);
}


#shader fragment
#version 450 core

layout(location = 0) out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;




uniform vec3 cameraPos;
uniform samplerCube skybox;

uniform float u_time;


struct Scene {
    mat4 model;
    mat4 view;
    mat4 proj;
    float gamma;
};

struct Camera {
    vec3 position;
    vec3 look_at;
};

struct Light {
    vec4 position;
    vec3 ambient_color;
    vec3 diffuse_color;
    vec3 specular_color;
    //float l_inear;
    //float quadratic;
    //float radius;
};

struct Material {
    vec3 ambient_color;
    vec3 diffuse_color;
    vec3 specular_color;
    float specular_shininess;
};

const unsigned int SCENES = 1;
const unsigned int CAMERAS = 1;
const unsigned int LIGHTS = 2;
const unsigned int MATERIALS = 1;

const unsigned int ACTIVE_SCENE = 0;
const unsigned int ACTIVE_CAMERA = 0;
const unsigned int ACTIVE_MATERIAL = 0;

layout(std140, binding = 5) uniform CameraProperties
{
    Camera cameras[CAMERAS];
};

layout(std140, binding = 3) uniform LightProperties
{
    Light lights[LIGHTS];
};

layout(std140, binding = 4) uniform MaterialProperties
{
    Material material;
};

layout(std140, binding = 6) uniform SceneProperties
{
    Scene scenes[SCENES];
};

vec4 calc_color(Scene scene, Light light, Material mat, vec4 camera_position)
{
    // ambient
    vec3 ambient = light.ambient_color * mat.ambient_color;
    vec3 light_pos = vec3(scene.view * light.position);
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light_pos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse_color * (diff * mat.diffuse_color);
    // specular

    vec3 camera_pos = vec3(scene.view * camera_position);
    vec3 viewDir = normalize(camera_pos - FragPos);
    vec3 reflectDir = reflect(-lightDir, viewDir);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), mat.specular_shininess);
    vec3 specular = light.specular_color * (spec * mat.specular_color);

    //vec3 result = (ambient + diffuse + specular) * vec3(u_Color);
    //vec3 result = (ambient + diffuse + specular) * 1.1f;
    vec3 result = ambient + diffuse + specular;
    return vec4(result, 1.0);
}

void main()
{    
    vec3 I = normalize(FragPos - cameraPos);
    vec3 R = reflect(I, normalize(Normal));
    vec4 sky_color = vec4(texture(skybox, R).rgb, 1.0); // FragColor

    Scene scene = scenes[ACTIVE_SCENE];
    vec4 camera_position = vec4(cameras[ACTIVE_CAMERA].position, 1.0);

    // times'ing things together looks the best! ... so far!
    FragColor = vec4(1.0f);
    for (unsigned int n = 0;n < LIGHTS;n++)
    {
        //FragColor = FragColor + calc_color(lights[n], material);
        FragColor = FragColor * calc_color(scene, lights[n], material, camera_position);
    }
    //FragColor = FragColor / LIGHTS;
    FragColor = FragColor * LIGHTS + (sky_color / 2.0f);
    FragColor.rgb = pow(FragColor.rgb, vec3(1.0 / scene.gamma));

    FragColor = sky_color;
}