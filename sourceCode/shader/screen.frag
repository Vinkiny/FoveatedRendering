#version 410
in vec2 TexCoords;

uniform sampler2D colorRenderTexture;
uniform vec3 gazePosition;
uniform vec4 gazeDirection;


out vec4 FragColor;

// parameter for foveated rendering
//float focusRadius = 0.1f; // size of center area (focus area)
float focusRadius = 0.3f; // size of center area (focus area)
float decay = 0.07f; //rate from focus point to edge (center to blur area) 
float contrastPower = 1.5f; // make contrast effect more clear or strong
float maxBlur = 9.0f; // control blur effect, bigger, blur more clear

//center of image
vec2 center = vec2(0.5f);

//luminance contrast parameters
float epsilon = 0.001;
vec3 grayScale = vec3(0.299f, 0.587, 0.114f);
float luminanceContrastSum = 0;
float luminanceContrastAvg = 0;

void main()
{
    // render to the texture
    //FragColor = vec4(vec3(texture(colorRenderTexture,TexCoords)),1.0);     

    //-------mipmap------foveated rendering
    // current pixel coordinate
    vec2 uv = gl_FragCoord.xy/textureSize(colorRenderTexture,0);
    // distance from current pixel to center point
    float dis = distance(center,uv);

    // get the mix mipmap level of current texture 
    // textureSize(colorRenderTexture,0), colorRenderTexture at mipmap level 0
    float maxMipmapLevel =float(log2(textureSize(colorRenderTexture,0)));

    // lod: the position of current pixel to the center area
    // range from 0 to 1
    // 0 is the surrounding area (close low mipmap level), 1 is the center area (close high mipmap level)
    // distance base
    float lod = clamp((dis/focusRadius), 0.0, 1.0);   
    // decay base
    //float lod = clamp((dis/decay)-focusRadius, 0.0, 1.0);

    //-------standard foveated rendering-------
    float mipmapLevel_standardFoveated = maxMipmapLevel/2 * lod;//:mipmap level
    vec4 color_standardFoveated = textureLod(colorRenderTexture,uv,float(mipmapLevel_standardFoveated));
    //FragColor = color_standardFoveated;


    //-------luminance contrast loop-------
    for(unsigned int i = 4;i < int(maxMipmapLevel)-2; i++)
    {
        vec4 a = textureLod(colorRenderTexture,uv,float(i));
        vec4 b = textureLod(colorRenderTexture,uv,float(i+1));
        vec4 c = textureLod(colorRenderTexture,uv,float(i+2));
        
        float a_gray = dot(a.rgb, grayScale);
        float b_gray = dot(b.rgb, grayScale);
        float c_gray = dot(c.rgb, grayScale);//average luminance
    
        luminanceContrastSum += abs(a_gray-b_gray)/(c_gray + epsilon);
    }
    
    float result = clamp(lod - luminanceContrastSum*contrastPower,0,1);
    
    float mipmapLevel = (maxBlur) * result;// get the mipmap level of current pixel of foveated rendering
    vec4 color = textureLod(colorRenderTexture,uv,float(mipmapLevel));
    //vec4 color = textureLod(colorRenderTexture,uv,float(mipmapLevel/maxMipmapLevel));
    //FragColor = vec4(vec3(mipmapLevel/maxMipmapLevel),1);//mipmap luminance distance
    //FragColor = vec4(vec3(mipmapLevel),1);

    //final rendering
    FragColor = color; 

    //-------test------
    //FragColor = vec4(vec3(result),1.0); 
    //FragColor = vec4(vec3(lod),1.0); 
    //FragColor = vec4(vec3(dis),1.0); 
    //FragColor = vec4(vec3(luminanceContrastSum),1.0); 
}