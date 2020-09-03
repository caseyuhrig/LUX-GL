#shader vertex
#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

void main()
{
    TexCoords = aTexCoords;
    gl_Position = vec4(aPos, 1.0);
}



#shader fragment
#version 450 core
out vec4 FragColor;

in vec2 TexCoords;

//uniform sampler2D scene;
//uniform sampler2D bloomBlur;
uniform sampler2DMS scene;
//uniform sampler2DMS bloomBlur;
uniform sampler2D bloomBlur;

uniform bool bloom;
uniform float exposure;
uniform int u_Samples;


vec4 textureMS(sampler2DMS tex, ivec2 texCoord, int samples)
{
    vec4 result = vec4(0.0);
    for (int i = 0; i < samples; i++)
        result += texelFetch(tex, texCoord, i);

    result /= float(samples);

    return result;
}

void main()
{   
  
        ivec2 texSize = textureSize(scene);
        ivec2 texCoord = ivec2(TexCoords * texSize);

        vec3 hdrColor = textureMS(scene, texCoord, u_Samples).rgb;      
        //vec3 bloomColor = textureMS(bloomBlur, texCoord, u_Samples).rgb;
        vec3 bloomColor = texture(bloomBlur, TexCoords).rgb;
        vec3 result = hdrColor;
        
        if (bloom && length(bloomColor) > 0)
        {
            hdrColor += bloomColor; // additive blending
            // tone mapping
            result = vec3(1.0) - exp(-hdrColor * exposure);
            //result = vec3(1.0, 0.0, 0.0);
        }
        
        FragColor = vec4(result, 1.0);
   
}

