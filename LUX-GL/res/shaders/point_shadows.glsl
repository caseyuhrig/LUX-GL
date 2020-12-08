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

layout(std430, binding = 10) buffer CameraSpec
{
    mat4 Projection;
    mat4 View;
    mat4 ViewProjection;
    vec3 Position;
    vec3 LookAt;
    float zFar;
    float zNear;
    float AspectRatio;
    float Angle;
    vec3 Up;
    int ViewportWidth;
    int ViewportHeight;
} camera;

layout(std430, binding = 13) buffer TransformsSpec
{
    mat4 Transform[10];
} transform;

uniform mat4 u_Model;
uniform bool u_ReverseNormals;

void main()
{
    mat4 model = u_Model;
    //mat4 model = transform.Transform[0];

    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
    if(u_ReverseNormals) // a slight hack to make sure the outer large cube displays lighting from the 'inside' instead of the default 'outside'.
        vs_out.Normal = transpose(inverse(mat3(model))) * (-1.0 * aNormal);
    else
        vs_out.Normal = transpose(inverse(mat3(model))) * aNormal;
    vs_out.TexCoords = aTexCoords;
    gl_Position = camera.Projection * camera.View * model * vec4(aPos, 1.0);
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

layout(std430, binding = 10) buffer CameraSpec {
    mat4 Projection;
    mat4 View;
    mat4 ViewProjection;
    vec3 Position;
    vec3 LookAt;
    float zFar;
    float zNear;
    float AspectRatio;
    float Angle;
    vec3 Up;
    int ViewportWidth;
    int ViewportHeight;
} camera;

layout(std430, binding = 11) buffer ShadowSpec {
    float ShadowBias;
    int ShadowSamples;
    float ShadowAmount;
    bool Shadows;
} shadow;

layout(std430, binding = 12) buffer LightSpec {
    vec3 LightPosition;
    vec3 LightColor;
} light;

layout(std430, binding = 13) buffer MaterialSpec {
    vec3 Color;
    vec3 Ambient;
    vec3 Diffuse;
    vec3 Specular;
    float Shininess;
} material;


//uniform sampler2D diffuseTexture;
uniform samplerCube depthMap;
//uniform samplerCubeShadow depthMap;
uniform vec4 u_Color;


uniform vec3 u_CameraPosition;
uniform vec3 u_LightPosition;
uniform vec3 u_LightColor;

vec3 lightPos = light.LightPosition;
//vec3 lightPos = u_LightPosition;
vec3 lightColor = light.LightColor;
//vec3 lightColor = u_LightColor;
vec3 camPos = camera.Position;
//vec3 camPos = u_CameraPosition;
float camZFar = camera.zFar;
//float camZFar = 2000.0f;


vec3 color = u_Color.rgb; //material.Color; //u_Color.rgb;
//vec3 materialAmbient = vec3(0.3); //vec3(1.0f, 0.5f, 0.31f);
//vec3 materialDiffuse = vec3(1.0); //vec3(1.0f, 0.5f, 0.31f);
//vec3 materialSpecular = vec3(1.0); //vec3(0.5f, 0.5f, 0.5f);
//float materialShininess = 64.0; // [32.0f]
vec3 materialAmbient = material.Ambient;
vec3 materialDiffuse = material.Diffuse;
vec3 materialSpecular = material.Specular;
float materialShininess = material.Shininess;


float shadowAlpha = shadow.ShadowAmount; // [0.5] 0.75 0..1
int shadow_samples = shadow.ShadowSamples; // [10] 1..+
float shadow_bias = shadow.ShadowBias; // [0.3] 0..1
bool doShadows = shadow.Shadows; // [true]


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
    vec3 fragToLight = fragPos - lightPos;
    // now get current linear depth as the length between the fragment and light position
    float currentDepth = length(fragToLight); 
    float amount = 0.0; // shadow amount
    float viewDistance = length(camPos - fragPos);
    float diskRadius = (1.0 + (viewDistance / camZFar)) / 25.0;
    for(int i = 0; i < shadow_samples; ++i)
    {
        float closestDepth = texture(depthMap, fragToLight + gridSamplingDisk[i] * diskRadius).r;
        closestDepth *= camZFar;   // undo mapping [0;1]
        if(currentDepth - shadow_bias > closestDepth)
            amount += 1.0;
    }
    amount /= float(shadow_samples);

    // display closestDepth as debug (to visualize depth cubemap)
    //FragColor = vec4(vec3(closestDepth / far_plane), 1.0);    
        
    return amount;
}

vec4 CloDep(vec3 fragPos)
{
    vec3 fragToLight = fragPos - lightPos;
    float closestDepth = texture(depthMap, fragToLight).r;
    return vec4(vec3(closestDepth / camZFar), 1.0);
}

void main()
{
//materialAmbient = vec3(0.3);
//materialDiffuse = vec3(1.0);
//materialSpecular = vec3(1.0);


    vec3 normal = normalize(fs_in.Normal);
    // ambient
    //vec3 ambient = ambientAmount * color;
    vec3 ambient = lightColor * materialAmbient;
    // diffuse
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = lightColor * (diff * materialDiffuse);
    // specular
    vec3 viewDir = normalize(camera.Position - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    //float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), materialShininess);
    vec3 specular = lightColor * (spec * materialSpecular);    
    // calculate shadow
    //bool doShadows = true; //shadow.Shadows;
    float shadowAmount = doShadows ? ShadowCalculation(fs_in.FragPos) : 0.0;
    vec3 lighting = (ambient + (1.0 - shadowAmount * shadowAlpha) * (diffuse + specular)) * color;

    //FragColor = vec4(lighting, u_Color[3]);
    vec3 outColor = lighting;
    outColor = outColor / (outColor + vec3(1.0));
    //outColor = pow(outColor, vec3(1.0/2.2));
    
    FragColor = vec4(outColor, u_Color[3]);
    
    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0)
        BrightColor = vec4(FragColor.rgb, u_Color[3]);
    else
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0); // black

    // adds a cool hue to everything!
    BrightColor = vec4(FragColor.rgb, 1.0);
}