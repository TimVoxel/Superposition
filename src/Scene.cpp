#include <Scene.hpp>
#include <utility>
#include <Math.hpp>
#include <ParticleSystem.hpp>
#include <ParticleSystemLink.hpp>
#include <WaveFunction.hpp>
#include <stdexcept>

Scene::Scene(const std::string& name, std::vector<std::unique_ptr<SceneObject>> objects)
    : objects_(std::move(objects)), name_(std::move(name)) 
{
}

void Scene::start(float currentTime)
{
    for (auto& object : objects_)
    {
        object->start(currentTime);
    }
}

void Scene::update(float deltaTime, float currentTime)
{
    for (auto& object : objects_)
    {
        object->update(deltaTime, currentTime);
    }
}

const std::vector<std::unique_ptr<SceneObject>>& Scene::objects() const
{
    return objects_;
}

Scene Scene::fromJson(const nlohmann::json& json)
{
    std::vector<std::unique_ptr<SceneObject>> objects;
    const std::string name = json.at("name");

    for (const auto& objectJson : json.at("objects"))
    {
        const std::string type = objectJson.at("type");

        if (type == "particleSystem")
        {
            const auto config = objectJson.get<ParticleSystemConfig>();
            objects.push_back(std::make_unique<ParticleSystem>(config));
        }
        else if (type == "particleSystemLink")
        {
            const auto firstConfig = objectJson.at("first").get<ParticleSystemConfig>();
            const auto secondConfig = objectJson.at("second").get<ParticleSystemConfig>();
            const auto waveFunction = objectJson.at("waveFunction").get<WaveFunction>();

            objects.push_back(
                std::make_unique<ParticleSystemLink>(
                    ParticleSystem(firstConfig),
                    ParticleSystem(secondConfig),
                    waveFunction
                )
            );
        }
        else
        {
            throw std::runtime_error("Unknown scene object type: " + type);
        }
    }
    return Scene(name, std::move(objects));
}

const std::string& Scene::name() const { return name_; }