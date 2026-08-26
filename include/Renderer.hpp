#pragma once
#include <Shader.hpp>
#include <vector>
#include <Particle.hpp>
#include <Color.hpp>

class Renderer
{
public:
    Renderer(const Shader& shader, Color pointColor, float pointSizePercentage, float pointSizeIncreasePercentage);
    static bool init();
    static bool initOpenGL(Color clearColor);
    void render(const int width, const int height);
    void render(const int width, const int height, const std::vector<Particle>& particles);
    std::vector<unsigned char> capture(int width, int height);
    static void terminate();
private:
    const Shader& shader_;
    GLuint VAO_;
    GLuint VBO_;
    Color pointColor_;
    float pointSizePercentage_;
    float pointSizeIncreasePercentage_;
};