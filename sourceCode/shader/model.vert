#version 410
in vec3 modelPositions;
//in vec3 modelNormals;
in vec2 modelTextures;

uniform mat4 ModelViewProjection;

out vec2 TextureCoords;

void main()
{
    TextureCoords = modelTextures;
    gl_Position = ModelViewProjection * vec4(modelPositions, 1.0);
}