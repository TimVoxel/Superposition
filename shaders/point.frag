#version 430 core
#define MAX_POINTS 16

in vec2 uv;
out vec4 fragColor;

uniform int pointCount;
uniform vec2 points[MAX_POINTS];
uniform vec2 resolution;
uniform float time;

void main()
{
    float cloud = 0.0;

    for (int i = 0; i < pointCount; i++)
    {
        vec2 offset = uv - points[i];
        offset.x *= resolution.x / resolution.y;
        float distance = length(offset);
        cloud += exp(-distance * distance * 100.0);
    }

    fragColor = vec4(
        cloud,
        0.0,
        cloud,
        1.0
    );
}