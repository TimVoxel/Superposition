#version 430 core

in float particleAge;

uniform vec4 pointColor;
out vec4 fragColor;

void main()
{
    vec2 offset = gl_PointCoord - vec2(0.5);
    float distance = length(offset);

    float pointAlpha = exp(-distance * distance * 18.0);

    float fadeIn = smoothstep(0.0, 0.025, particleAge);
    float fadeOut = 1.0 - particleAge;

    float alpha = pointAlpha * fadeIn * fadeOut;
    fragColor = vec4(pointColor.x, pointColor.y, pointColor.z, pointColor.w * alpha);
}