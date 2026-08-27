#pragma once
#include <Shader.hpp>
#include <vector>
#include <Color.hpp>
#include <RenderPoint.hpp>

class Renderer
{
public:
    Renderer(const Shader& shader);
    static bool init();
    static bool initOpenGL(Color clearColor);
    void render(const int width, const int height, const std::vector<RenderPoint>& points);
    std::vector<unsigned char> capture(int width, int height);
    static void terminate();
private:
    const Shader& shader_;
    GLuint VAO_;
    GLuint VBO_;
};