#include <SceneObject.hpp>
#include <ParticleSystem.hpp>
#include <ParticleSystemLink.hpp>
#include <Container.hpp>

void SceneObject::start(float currentTime)
{
    onStart(currentTime);

    for (std::unique_ptr<SceneObject>& child : children_)
    {
        child->start(currentTime);
    }
}

void SceneObject::update(float deltaTime, float currentTime)
{
    onUpdate(deltaTime, currentTime);

    for (std::unique_ptr<SceneObject>& child : children_)
    {
        child->update(deltaTime, currentTime);
    }
}

std::unique_ptr<SceneObject> SceneObject::fromJson(const nlohmann::json& json)
{
    const std::string type = json.at("type");

    std::unique_ptr<SceneObject> object;

    if (type == "particleSystem")
    {
        object = ParticleSystem::fromJson(json);
    }
    else if (type == "particleSystemLink")
    {
        object = ParticleSystemLink::fromJson(json);
    }
    else if (type == "container")
    {
        object = std::make_unique<Container>();
    }
    else
    {
        throw std::runtime_error("Unknown scene object type: " + type);
    }

    object->transform() = json.at("transform").get<Transform>();
    for (const auto& childJson : json.value("children", nlohmann::json::array()))
    {
        object->addChild(SceneObject::fromJson(childJson));
    }
    return object;
}

const Transform SceneObject::worldTransform() const
{
    if (parent_ == nullptr)
    {
        return transform_;
    }

    const Transform parentTransform = parent_->worldTransform();

    return {
        .position = {
            parentTransform.position.x + transform_.position.x * parentTransform.scale.x,
            parentTransform.position.y + transform_.position.y * parentTransform.scale.y
        },

        .scale = {
            parentTransform.scale.x * transform_.scale.x,
            parentTransform.scale.y * transform_.scale.y
        }
    };
}