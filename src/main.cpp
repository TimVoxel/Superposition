#include <glad/gl.h>
#include <iostream>
#include <shader.hpp>
#include <math.h>
#include <particle.hpp>
#include <random>
#include <vector>
#include <Renderer.hpp>
#include <Window.hpp>
#include <ParticleSystem.hpp>
#include <Config.hpp>
#include <nlohmann/json.hpp>
#include <fstream>

Config loadConfig()
{
    std::ifstream file("config/config.json");
    if (!file.is_open())
    {
        std::cout << "Unable to open config file config/config.json\n";
        Window::terminate();
        return {};
    }
    nlohmann::json json = nlohmann::json::parse(file);
    return configFromJson(json);
}

int main()
{
    Config config = loadConfig();

    if (!Window::init())
    {
        return -1;
    }
    
    std::unique_ptr<Window> window = Window::create(config.width, config.height, "Superposition");
    if (window == nullptr)
    {
        Window::terminate();
        return -1;
    }
    window->makeContextCurrent();

    if (!Renderer::init(glfwGetProcAddress, config.clearColor))
    {
        Window::terminate();
        return -1;
    }
    
    Shader shader("shaders/point.vert", "shaders/singlePoint.frag");
    Renderer renderer(shader, config.pointColor, config.pointSize, config.sizeIncrease);
    ParticleSystem particleSystem(config.durationSeconds, config.maxSpawnRatePS);
    float previousTime = static_cast<float>(glfwGetTime());

    while (!window->shouldClose())
    {
        window->update();
        float currentTime = static_cast<float>(glfwGetTime());
        float deltaTime = currentTime - previousTime;
        previousTime = currentTime;
        particleSystem.update(deltaTime);

        renderer.render(window->width(),window->height(), particleSystem.particles());
        window->swapBuffers();
    }
    Window::terminate();
    return 0;
}