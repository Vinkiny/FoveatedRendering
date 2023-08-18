#version 410
in vec3 screenPositions;
in vec2 screenUV;

uniform mat4 ModelViewProjection;

out vec2 TexCoords;

void main()
{
    TexCoords = screenUV;
    gl_Position = vec4(screenPositions,1.0); // ? maybe change
    // gl_Position = vec4(screenPositions.x,screenPositions.y,0.0,1.0);
}