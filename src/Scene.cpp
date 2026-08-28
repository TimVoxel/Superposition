#include <Scene.hpp>
#include <utility>

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
        objects.push_back(SceneObject::fromJson(objectJson));
    }
    return Scene(name, std::move(objects));
}

const std::string& Scene::name() const { return name_; }