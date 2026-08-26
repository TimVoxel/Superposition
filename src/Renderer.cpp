#include "Renderer.hpp"
#include <vector>
#include <Particle.hpp>

Renderer::Renderer(const Shader& shader, Color pointColor, float pointSizePercentage, float pointSizeIncreasePercentage)
    : shader_(shader),
    pointColor_(pointColor),
    pointSizePercentage_(pointSizePercentage),
    pointSizeIncreasePercentage_(pointSizeIncreasePercentage)
{
    glGenVertexArrays(1, &VAO_);
    glBindVertexArray(VAO_);

    glGenBuffers(1, &VBO_);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 3 * sizeof(float), reinterpret_cast<void*>(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

bool Renderer::init(GLADloadfunc addrFunc, Color clearColor)
{
    if (!gladLoadGL(addrFunc))
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

void Renderer::render(int width, int height)
{
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT);
    shader_.use();
    glBindVertexArray(VAO_);
    glDrawArrays(GL_POINTS, 0, 4);
}

void Renderer::render(
    const int width,
    const int height,
    const std::vector<Particle>& particles
)
{
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT);

    std::vector<float> positions;
    
    int particleCount = particles.size();
    positions.resize(particleCount * 3);

    for (int i = 0; i < particleCount; i++)
    {
        int pi = i * 3;
        positions[pi] = particles[i].x;
        positions[pi + 1] = particles[i].y;
        positions[pi + 2] = particles[i].age / particles[i].lifetime;
    }

    glBindVertexArray(VAO_);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_);
    glBufferData(
        GL_ARRAY_BUFFER,
        positions.size() * sizeof(float),
        positions.data(),
        GL_DYNAMIC_DRAW
    );
    shader_.use();
    shader_.setVec2("resolution", static_cast<float>(width), static_cast<float>(height));
    shader_.setVec4("pointColor", pointColor_.r, pointColor_.g, pointColor_.b, pointColor_.a);
    shader_.setFloat("pointSizePercentage", pointSizePercentage_);
    shader_.setFloat("pointSizeIncreasePercentage", pointSizeIncreasePercentage_);
    glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(particles.size()));
}