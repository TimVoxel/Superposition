#include <Scene.hpp>
#include <utility>

Scene::Scene(const std::string& name, float durationSeconds, std::vector<std::unique_ptr<SceneObject>> objects)
    : objects_(std::move(objects)), name_(std::move(name)), durationSeconds_(durationSeconds)
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
    const float durationSeconds = json.at("durationSeconds").get<float>();

    for (const auto& objectJson : json.at("objects"))
    {
        objects.push_back(SceneObject::fromJson(objectJson));
    }
    return Scene(name, durationSeconds, std::move(objects));
}

const std::string& Scene::name() const { return name_; }
float Scene::durationSeconds() const { return durationSeconds_; }
