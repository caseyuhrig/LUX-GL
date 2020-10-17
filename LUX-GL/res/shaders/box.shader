#shader vertex
#version 450 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
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

uniform mat4 u_2D_proj;
uniform mat4 u_2D_view;
uniform mat4 u_2D_model;


// https://learnopengl.com/Lighting/Basic-Lighting
// normal help, calc normals prior to the GPU
// Normal = mat3(transpose(inverse(model))) * aNormal; 

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


    //FragPos = vec3(u_model * vec4(a_position, 1.0));
    // OMG very important!
    //Normal = vec3(u_model * vec4(a_normal, 0.0));
    //gl_Position = u_proj * u_view * vec4(FragPos, 1.0);
};



#shader fragment
#version 450 core


layout(location = 0) out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

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

/*
float rand(vec2 co) {
    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}
*/

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

// specular
// http://shdr.bkcore.com/#1/dVNNb6MwEP0rI07QEpJt0z00yqlSb7va0142q8gBA46MjWyTj1b97zsYJ9hscrBAfm9m3swbf0aFzLuGCqOj1z9Rq2jONJMCalbVLZRcErPaiANRZyYqOND8GcpfUjODpP+An1I1hOP1RuRSaDPEA2l2DCtsc0nLEgDWsMieXlYhSbc07zhRF9YavmWLexx6asEmen7ySFYEcFRutgXDVnqRSOrv4xmmS91JVjCfgzakJ2AHQ8ytNLnkUsFQy6YJsoQRcrfHkmOIF7HIvuNxdc+Uc3mcMV2DVERUtJ/XQbICGsJEnGzE50bAkJRjFmGnyj5oPGmtF+B4IuA5IzycBvjsaqFHqX0KxPTRxl/h0UTb3nrq6kPQ/5jW87UPXE+NfoBWHuOGnGIcUFpIE4u0TpLU9zpBlueHk1Xx7bsi1ZvLi9WWcaDyMaydgjPtK0rvbjoxRrFdZ+igvh1XfYKIy653gpX4j+aZy+0PgtRTiC2hkQXlvxk93oZbJfeDr1f8zuu6+xpvrZELC/z3VeJoRbgty75r5E/0ItGO+DKS6zhh1IBRCGen88fKOeQjkwat9dr68TeNbLXodfjqed7taLbX0dc/
// http://shdr.bkcore.com/#1/jVVtb9s4DP4rRD7FruPGXtLDGuSAod2AYVgx3A7DgPVQuI4Sq3Mkw5ITO8P++1Fvfmsz1GhqmaRIinr48Ndkw9NqT5gUk+sfk6IkKRWUM8joLitgm/NEru5ZxeiWl3vzDZLuSU94IGkMJRE8ryRuRc0hKRvKdkrzBrZfuKAvKu5wd5KPxWVy/I6ye5ZyJqSRQY7pyG8R4LPWouk8nAcR/vDtreDyEoRMVBR0ZaydA5OzFn0E4wD3rWD4oAPO8gbwRLCh220lCKR8X3CGtRm6SvaPFIU32tU8jJarod5ud/q/RmpRkLTKk/ImOpvJB0ziz8dxTt4rJ1cL9HJGH6sgVz19r543PO/qGdl6RqqeQ2P++ERSY90znodXXfEbkuf8OKMiA14mbEfUBWpgPOaUsS8ZZ7tbWk61Qx0cv4LB3TDpvj8l7WrTroRbiYwyyogQ3j37dc/AJCkwNabxRE9k6iKok1iDw8Bg1qKyZ8IGJhaePX020B8uhNENbh1NPiVwgZmD3x4Ml/uk1pCFDZdTFoDw+pvVdeFOPCWaFvw4HZlnnhf0Dq53lkRWJdPdN7XBA+1IqX/r+nO6wcCUTV2pLIDZLidgeyFehnPfdrQ9p2m2GHrdJtCJ3uYFs3m4DFIu7Pf4GBrasUV+i+Y9P6gG155hNsOsfhKQGRVwIiUHyeEnIYVpwefIbzNDDPLyq8L8EFaWHhQeMedAV0497aJruqBrHS9sfNcIq1GM+EyM+BUx4i5G/IcY727PnsMAJ3BMEzhKscFV/MitwAtrv21RHWOXP3wok52OYm5wMbUhL2wJ7Tt2/Q6gfnhbX2VJCzLbUJEm5QbIdouO1ZUhG6o5AbyS7X0h5yuI8Q3J1Xa61UBZYpa+ovGw9v5GtHhgvSlgToKzUyaRGPuxksQUqejmxkjD3OBwI2ifSCf9nKBpPdQtTIrfKDm+rC5KruqHwVr9a0ZVf7SdGZJDWi7o+k0YLaLl27ErneD3F6V3ZyekJth/OPYLiW+n+gv/PVAW9Du9x5NGjzfooKjWYb3u2tk3DsJ61rW8kzWDXc36uUV98dxTM2IrG/4MQ5k6rHt3D/q8Q4mpy9riwGDX4vfGwlRmBNk4V4TpICskL0AeqZBYyi5YWJ+wSdoytkJstqVfUN8KGl+dN5pbuvTM4PsXWcp2hrofFcl/qoT0Dbup+HDtdTm/FOzu1cGUS/wTdF8ggycCg+AtG9akLFWUij8cwEIjEU1LbtjUVcgieTDJ+o2jRpXOqZ17C9WIaD9qITTUvGISDqDlkbYpcA/uDOvm5Eiprxl1nJ57QhPEf8FEO51cm7e4FNUpYYw8KLoIn8Tk9/8=
// https://thebookofshaders.com/05/
// http://tobyschachman.com/Shadershop/

float random(in vec2 st) {
    return fract(sin(dot(st.xy,
        vec2(12.9898, 78.233)))
        * 43758.5453123);
}

void main()
{
    Scene scene = scenes[ACTIVE_SCENE];
    vec4 camera_position = vec4(cameras[ACTIVE_CAMERA].position, 1.0);

    // times'ing things together looks the best! ... so far!
    FragColor = vec4(1.0f);
    //for (unsigned int n = 0;n < LIGHTS;n++)
    //{
        //FragColor = FragColor + calc_color(lights[n], material);
        //FragColor = FragColor * calc_color(scene, lights[n], material, camera_position);
        FragColor = FragColor * calc_color(scene, lights[0], material, camera_position);
    //}
    //FragColor = FragColor / LIGHTS;
    //FragColor = FragColor * LIGHTS;

    //FragColor.rgb = pow(FragColor.rgb, vec3(1.0 / scene.gamma));

    //vec2 st = gl_FragCoord.xy / vec2(1500,900);
    //float r = random(st * 1.0) * 0.25;
    //FragColor.rgb = FragColor.rgb + r;

    /*
    float theta = u_time * 2.0;                    // (2.0 = speed)

    vec3 dir1 = vec3(cos(theta), 0, sin(theta)) * 10.0f;
    vec3 dir2 = vec3(sin(theta), 0, cos(theta)) * 10.0f;
    
    float diffuse1 = pow(dot(Normal, dir1), 2.0);
    float diffuse2 = pow(dot(Normal, dir2), 2.0);
    
    vec3 col1 = diffuse1 * vec3(1, 0, 0);
    vec3 col2 = diffuse2 * vec3(0, 0, 1);
    */
    //FragColor = vec4(col1 + col2, 1.0);
    //FragColor = vec4(FragColor.rgb * (col1 + col2), 1.0);
    //gl_FragColor
};