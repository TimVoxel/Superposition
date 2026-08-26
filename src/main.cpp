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
#include <exception>
#include <RunMode.hpp>
#include <VideoWriter.hpp>
#include <utility>

std::optional<Config> loadConfig()
{
    std::ifstream file("config/config.json");
    if (!file.is_open())
    {
        return std::nullopt;
    }
    nlohmann::json json = nlohmann::json::parse(file);
    return configFromJson(json);
}

std::optional<Shader> loadShader(const std::string& vertPath, const std::string& fragPath)
{
    try
    {
        return Shader::tryLoad(vertPath, fragPath);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return std::nullopt;
    }
}

RunMode getRunMode(int argc, char** argv)
{
    RunMode mode = RunMode::Display;

    for (int i = 1; i < argc; ++i)
    {
        std::string argument = argv[i];

        if (argument == "-r")
        {
            mode = RunMode::Render;
        }
        else if (argument == "-d")
        {
            mode = RunMode::Display;
        }
        else 
        {
            std::cout << "Unknown option \"" << argument << "\": -r, -d\n";
        }
    }
    return mode;
}

int safeExit(int code, const std::optional<std::string> errorMessage)
{
    if (errorMessage.has_value())
    {
        std::cerr << errorMessage.value();
    }
    Renderer::terminate();
    return code;
}

int runDisplay(Window& window, Renderer& renderer, ParticleSystem& particleSystem, const Config& config)
{
    float startTime = glfwGetTime();
    float previousTime = startTime;
    
    while (!window.shouldClose())
    {
        window.update();
        
        float currentTime = static_cast<float>(glfwGetTime());
        float deltaTime = currentTime - previousTime;
        previousTime = currentTime;

        particleSystem.update(deltaTime, (currentTime - startTime) < config.durationSeconds);
        renderer.render(window.width(), window.height(), particleSystem.particles());
        window.swapBuffers();
    }
    return safeExit(0, std::nullopt);
}

int runRender(Window& window, Renderer& renderer, ParticleSystem& particleSystem, const Config& config)
{
    int fps = config.fps;
    float deltaTime = 1.0f / static_cast<float>(fps);
    int spawnParticleFrames =config.durationSeconds * fps; 
    int totalFrames = spawnParticleFrames + config.fadeoutSeconds * fps;

    int width = config.width;
    int height = config.height;

    VideoWriter video;

    if (!video.open(width, height, fps, "output/video.mp4"))
    {
        return safeExit(-1,"Failed to open video writer\n");
    }

    for (int frame = 0; frame < totalFrames; ++frame)
    {
        particleSystem.update(deltaTime, frame < spawnParticleFrames);
        renderer.render(width, height, particleSystem.particles());
        auto pixels = renderer.capture(width, height);
        video.writeFrame(pixels.data(), pixels.size());
    }

    video.close();
    return safeExit(0, std::nullopt);
}

int main(int argc, char** argv)
{
    RunMode mode = getRunMode(argc, argv);
 
    std::optional<Config> config = loadConfig();
    if (!config.has_value())
    {
        return safeExit(-1, "Unable to open config file \"config/config.json\n\"");
    }

    if (!Renderer::init())
    {
        return safeExit(-1, "Unable to initialize rendering environment");
    }

    std::unique_ptr<Window> window = Window::create(config->width, config->height, "Superposition");
    if (window == nullptr)
    {
        return safeExit(-1, "Unable to open window\n");
    }
    window->makeContextCurrent();

    if (mode == RunMode::Render)
    {
        window->hide();
    }

    if (!Renderer::initOpenGL(config->clearColor))
    {
        return safeExit(-1, "Unable to initialize OpenGL");
    }

    std::optional<Shader> shader = loadShader("shaders/point.vert", "shaders/singlePoint.frag");
    if (!shader.has_value())
    {
        return safeExit(-1, "Unable to start due to the shader not being loaded");
    }
    
    Renderer renderer(shader.value(), config->pointColor, config->pointSize, config->sizeIncrease);
    ParticleSystem particleSystem(config->durationSeconds, config->maxSpawnRatePS);
    
    if (mode == RunMode::Display)
    {
        return runDisplay(*window, renderer, particleSystem, config.value());
    }
    else
    {
        return runRender(*window, renderer, particleSystem, config.value());
    }
}