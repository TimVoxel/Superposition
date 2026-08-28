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
#include <VideoWriter.hpp>
#include <utility>
#include <Scene.hpp>
#include <Compositor.hpp>
#include <CommandLineArgs.hpp>

std::optional<Config> loadConfig(const std::string& path)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        return std::nullopt;
    }
    nlohmann::json json = nlohmann::json::parse(file);
    return json.get<Config>();
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

std::optional<Scene> loadScene(const std::string& path)
{
    try
    {
        std::ifstream file(path);

        if (!file.is_open())
        {
            return std::nullopt;
        }

        nlohmann::json json = nlohmann::json::parse(file);
        return Scene::fromJson(json);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Failed to load scene: " << e.what() << '\n';
        return std::nullopt;
    }
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

int runDisplay(Window& window, Renderer& renderer, Scene& scene, const Config& config)
{
    float startTime = glfwGetTime();
    float previousTime = startTime;

    Compositor compositor(scene);
    scene.start(startTime);
    
    while (!window.shouldClose())
    {
        window.update();
        
        float currentTime = static_cast<float>(glfwGetTime());
        float deltaTime = currentTime - previousTime;
        previousTime = currentTime;

        scene.update(deltaTime, currentTime);
        const auto& composed = compositor.compose();
        renderer.render(window.width(), window.height(), composed);
        window.swapBuffers();
    }
    return safeExit(0, std::nullopt);
}

int runRender(Window& window, Renderer& renderer, Scene& scene, const Config& config)
{
    int fps = config.fps;
    float deltaTime = 1.0f / static_cast<float>(fps);
    float startTime = static_cast<float>(glfwGetTime());
    int totalFrames = config.durationSeconds * fps; ;

    int width = config.width;
    int height = config.height;

    Compositor compositor(scene);
    VideoWriter video;
    if (!video.open(width, height, fps, "output/" + scene.name() + ".mp4"))
    {
        return safeExit(-1,"Failed to open video writer\n");
    }

    scene.start(startTime);

    for (int frame = 0; frame < totalFrames; ++frame)
    {
        float currentTime = startTime + frame * deltaTime;
        scene.update(deltaTime, currentTime);
        renderer.render(width, height, compositor.compose());
        auto pixels = renderer.capture(width, height);
        video.writeFrame(pixels.data(), width, height);
    }

    video.close();
    return safeExit(0, std::nullopt);
}

int main(int argc, char** argv)
{
    CommandLineArgs args = parseCommandLineArgs(argc, argv);
 
    std::optional<Config> config = loadConfig("resources/config.json");
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

    if (args.mode == RunMode::Render)
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
    
    Renderer renderer(shader.value());
    std::optional<Scene> scene = loadScene(args.scenePath);
    if (!scene.has_value())
    {
        return safeExit(-1, "Unable to load scene \"" + args.scenePath + "\"");
    }

    if (args.mode == RunMode::Display)
    {
        return runDisplay(*window, renderer, *scene, config.value());
    }
    else
    {
        return runRender(*window, renderer, *scene, config.value());
    }
}