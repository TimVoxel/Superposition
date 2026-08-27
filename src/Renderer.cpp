#include "Renderer.hpp"
#include <vector>
#include <glfw/glfw3.h>
#include <RenderPoint.hpp>

Renderer::Renderer(const Shader& shader) : shader_(shader)
{
    glGenVertexArrays(1, &VAO_);
    glBindVertexArray(VAO_);

    glGenBuffers(1, &VBO_);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_);

    constexpr GLsizei stride = 9 * sizeof(float);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, nullptr); // x, y
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(2 * sizeof(float))); // age    
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(3 * sizeof(float))); // size
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(4 * sizeof(float))); // size increase
    glEnableVertexAttribArray(3);

    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(5 * sizeof(float)));
    glEnableVertexAttribArray(4);
}

bool Renderer::init()
{
    if (!glfwInit())
    {
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    return true;
}

bool Renderer::initOpenGL(Color clearColor)
{
    if (!gladLoadGL(glfwGetProcAddress))
    {
        return false; 
    }

    glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_BLEND);
    glBlendFunc(
        GL_SRC_ALPHA,
        GL_ONE_MINUS_SRC_ALPHA
    );
    return true;
}

void Renderer::render(const int width, const int height, const std::vector<RenderPoint>& points)
{
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindVertexArray(VAO_);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_);
    glBufferData(
        GL_ARRAY_BUFFER,
        points.size() * sizeof(RenderPoint),
        points.data(),
        GL_DYNAMIC_DRAW
    );
    shader_.use();
    shader_.setVec2("resolution", static_cast<float>(width), static_cast<float>(height));
    glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(points.size()));
}

std::vector<unsigned char> Renderer::capture(int width, int height)
{
    std::vector<unsigned char> pixels(width * height * 4);
    glReadPixels(
        0,
        0,
        width,
        height,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        pixels.data()
    );
    return pixels;
}

void Renderer::terminate()
{
    glfwTerminate();
}