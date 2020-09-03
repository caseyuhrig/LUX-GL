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

/*
#NEWshader fragment
#version 450 core


uniform sampler2D image;

out vec4 color;

const float weights[] = float[](0.0024499299678342,
                                0.0043538453346397,
                                0.0073599963704157,
                                0.0118349786570722,
                                0.0181026699707781,
                                0.0263392293891488,
                                0.0364543006660986,
                                0.0479932050577658,
                                0.0601029809166942,
                                0.0715974486241365,
                                0.0811305381519717,
                                0.0874493212267511,
                                0.0896631113333857,
                                0.0874493212267511,
                                0.0811305381519717,
                                0.0715974486241365,
                                0.0601029809166942,
                                0.0479932050577658,
                                0.0364543006660986,
                                0.0263392293891488,
                                0.0181026699707781,
                                0.0118349786570722,
                                0.0073599963704157,
                                0.0043538453346397,
                                0.0024499299678342);

void main(void)
{
    vec4 c = vec4(0.0);
    ivec2 P = ivec2(gl_FragCoord.yx) - ivec2(0, weights.length() >> 1);
    int i;

    for (i = 0; i < weights.length(); i++)
    {
        c += texelFetch(image, P + ivec2(0, i), 0) * weights[i];
    }

    color = c;
    //if (length(c)>0) color = vec4(1,0,0,color[3]);
    //color = vec4(1,0,0,1);
}
*/

#shader fragment
#version 450 core
out vec4 FragColor;

in vec2 TexCoords;

//uniform sampler2D image;
uniform sampler2DMS image;
uniform int u_Samples;

uniform bool horizontal;
uniform float weight[5] = float[] (0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162);

vec4 textureMS(sampler2DMS tex, ivec2 texCoord, int samples)
{
    vec4 result = vec4(0.0);
    for (int i = 0; i < samples; i++)
        result += texelFetch(tex, texCoord, samples);

    result /= float(samples);

    return result;
}


void main()
{             
     ivec2 texSize = textureSize(image);
     ivec2 texCoord = ivec2(TexCoords * texSize);

     vec2 tex_offset = 1.0 / textureSize(image); // gets size of single texel
     vec3 result = textureMS(image, texCoord, u_Samples).rgb * weight[0];
     if(horizontal)
     {
         for(int i = 1; i < 5; ++i)
         {
            result += textureMS(image, texCoord + ivec2(tex_offset.x * i, 0.0), u_Samples).rgb * weight[i];
            result += textureMS(image, texCoord - ivec2(tex_offset.x * i, 0.0), u_Samples).rgb * weight[i];
         }
     }
     else
     {
         for(int i = 1; i < 5; ++i)
         {
             result += textureMS(image, texCoord + ivec2(0.0, tex_offset.y * i), u_Samples).rgb * weight[i];
             result += textureMS(image, texCoord - ivec2(0.0, tex_offset.y * i), u_Samples).rgb * weight[i];
         }
     }
     FragColor = vec4(result, 1.0);
     //if (length(result) > 0)
     //  FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
