#version 430 core

uniform float time;

layout (location = 0) in vec2 position;

void main()
{
    float offset = sin(time) * 0.5;
    gl_Position = vec4(position.x + offset, position.y, 0.0, 1.0);
}