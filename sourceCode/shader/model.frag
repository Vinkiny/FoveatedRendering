#version 410
in vec2 TextureCoords;

out vec4 ModelColor;

uniform sampler2D diffuseTexture;

void main()
{
    ModelColor = texture(diffuseTexture,TextureCoords);
    //ModelColor = vec4(vec3(1.0-texture(diffuseTexture,TextureCoords)),1.0);
    //ModelColor = vec4(0.4,0.36,0.8,1.0);
}