#shader vertex
#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} vs_out;

layout(std430, binding = 10) buffer SceneSpecification
{
    mat4 Projection;
    mat4 View;
    mat4 ViewProjection;
    vec3 Position;
    vec3 LookAt;
    float zFar;
    float zNear;
    // shadow parameters
    bool Shadows;
    float ShadowBias;
    int ShadowSamples;
    // light parameters
    vec3 LightPosition;
    vec3 LightColor;
} s_Spec;

uniform mat4 u_Model;
uniform bool u_ReverseNormals;

void main()
{
    vs_out.FragPos = vec3(u_Model * vec4(aPos, 1.0));
    if(u_ReverseNormals) // a slight hack to make sure the outer large cube displays lighting from the 'inside' instead of the default 'outside'.
        vs_out.Normal = transpose(inverse(mat3(u_Model))) * (-1.0 * aNormal);
    else
        vs_out.Normal = transpose(inverse(mat3(u_Model))) * aNormal;
    vs_out.TexCoords = aTexCoords;
    gl_Position = s_Spec.ViewProjection * u_Model * vec4(aPos, 1.0);
}

#shader fragment
#version 450 core

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

layout(std430, binding = 10) buffer SceneSpecification
{
    // camera parameters
    mat4 Projection;
    mat4 View;
    mat4 ViewProjection;
    vec3 Position;
    vec3 LookAt;
    float zFar;
    float zNear;
    // shadow parameters
    bool Shadows;
    float ShadowBias;
    int ShadowSamples;
    // light
    vec3 LightPosition;
    vec3 LightColor;
} s_Spec;


//uniform sampler2D diffuseTexture;
uniform samplerCube depthMap;
//uniform samplerCubeShadow depthMap;
uniform vec4 u_Color;


// array of offset direction for sampling
const vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

float ShadowCalculation(vec3 fragPos)
{
    // get vector between fragment position and light position
    vec3 fragToLight = fragPos - s_Spec.LightPosition;
    // now get current linear depth as the length between the fragment and light position
    float currentDepth = length(fragToLight); 
    float shadow = 0.0;
    float viewDistance = length(s_Spec.Position - fragPos);
    float diskRadius = (1.0 + (viewDistance / s_Spec.zFar)) / 25.0;
    for(int i = 0; i < s_Spec.ShadowSamples; ++i)
    {
        float closestDepth = texture(depthMap, fragToLight + gridSamplingDisk[i] * diskRadius).r;
        closestDepth *= s_Spec.zFar;   // undo mapping [0;1]
        if(currentDepth - s_Spec.ShadowBias > closestDepth)
            shadow += 1.0;
    }
    shadow /= float(s_Spec.ShadowSamples);

    // display closestDepth as debug (to visualize depth cubemap)
    //FragColor = vec4(vec3(closestDepth / far_plane), 1.0);    
        
    return shadow;
}

vec4 CloDep(vec3 fragPos)
{
    vec3 fragToLight = fragPos - s_Spec.LightPosition;
    float closestDepth = texture(depthMap, fragToLight).r;
    return vec4(vec3(closestDepth / s_Spec.zFar), 1.0);
}

void main()
{
    vec3 color = u_Color.rgb;
    vec3 normal = normalize(fs_in.Normal);
    // ambient
    vec3 ambient = 0.3 * color;
    // diffuse
    vec3 lightDir = normalize(s_Spec.LightPosition - fs_in.FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * s_Spec.LightColor;
    // specular
    vec3 viewDir = normalize(s_Spec.Position - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * s_Spec.LightColor;    
    // calculate shadow
    bool doShadows = s_Spec.Shadows;
    float shadow = doShadows ? ShadowCalculation(fs_in.FragPos) : 0.0;
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;

    FragColor = vec4(lighting, 1.0);
    
    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0)
        BrightColor = vec4(FragColor.rgb, u_Color[3]);
    else
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0); // black
}