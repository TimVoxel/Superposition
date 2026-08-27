#version 430 core

layout (location = 0) in vec2 position;
layout (location = 1) in float age;
layout (location = 2) in float size;
layout (location = 3) in float sizeIncrease;
layout (location = 4) in vec4 color;

uniform vec2 resolution;
out float pointAge;
out vec4 pointColor;

void main()
{
    float aspectRatio = resolution.x / resolution.y;
    gl_Position = vec4(
        position.x / aspectRatio,
        position.y,
        0.0,
        1.0
    );
    float startSize = resolution.x * size;
    float sizeDiff = resolution.x * sizeIncrease;
    float progress = smoothstep(0, 1, age);
    gl_PointSize = startSize + progress * sizeDiff;
    
    pointAge = age;
    pointColor = color;
}