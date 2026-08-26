#pragma once
#include <Shader.hpp>
#include <vector>
#include <Particle.hpp>
#include <Color.hpp>

class Renderer
{
public:
    Renderer(const Shader& shader, Color pointColor, float pointSizePercentage, float pointSizeIncreasePercentage);
    static bool init(GLADloadfunc addrFunc, Color clearColor);
    void render(const int width, const int height);
    void render(const int width, const int height, const std::vector<Particle>& particles);
private:
    const Shader& shader_;
    GLuint VAO_;
    GLuint VBO_;
    Color pointColor_;
    float pointSizePercentage_;
    float pointSizeIncreasePercentage_;
};