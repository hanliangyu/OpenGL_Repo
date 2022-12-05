// vertex shader
#version 410 core

// vertex position
layout (location = 0) in vec3 aPos;

out vec3 vPosition;

void main()
{
    vPosition = aPos;
    gl_Position = vec4(aPos, 1.0);
}