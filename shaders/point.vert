#version 430 core

layout (location = 0) in vec2 position;
layout (location = 1) in float age;

uniform vec2 resolution;
uniform float pointSizePercentage;
uniform float pointSizeIncreasePercentage;
out float particleAge;

void main()
{
    gl_Position = vec4(position, 0.0, 1.0);
    particleAge = age;
    float startSize = resolution.x * pointSizePercentage;
    float sizeDiff = resolution.x * pointSizeIncreasePercentage;
    float progress = smoothstep(0, 1, age);
    gl_PointSize = startSize + progress * sizeDiff;
}