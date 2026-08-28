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

void SceneObject::applyAnimation(float currentTime)
{
    animatedTransform_ = Animator::animate(transform_, animations_, currentTime);
}

void SceneObject::update(float deltaTime, float currentTime)
{
    applyAnimation(currentTime);
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
    for (const auto& animationJson :
        json.value("animations", nlohmann::json::array()))
    {
        object->addAnimation(Animation::fromJson(animationJson));
    }
    return object;
}

const Transform SceneObject::worldTransform() const
{
    if (parent_ == nullptr)
    {
        return animatedTransform_;
    }

    const Transform parentTransform = parent_->worldTransform();

    return {
        .position = {
            parentTransform.position.x + animatedTransform_.position.x * parentTransform.scale.x,
            parentTransform.position.y + animatedTransform_.position.y * parentTransform.scale.y
        },

        .scale = {
            parentTransform.scale.x * animatedTransform_.scale.x,
            parentTransform.scale.y * animatedTransform_.scale.y
        }
    };
}