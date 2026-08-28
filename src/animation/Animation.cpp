#include <animation/Animation.hpp>
#include <animation/SinAnimation.hpp>
#include <stdexcept>

static AnimationTarget targetFromJson(const nlohmann::json& json)
{
    AnimationTarget target = AnimationTarget::None;

    auto addTarget = [&](const std::string& value)
    {
        if (value == "position")
        {
            target = target | AnimationTarget::Position;
        }
        else if (value == "scale")
        {
            target = target | AnimationTarget::Scale;
        }
        else
        {
            throw std::runtime_error("Unknown animation target: " + value);
        }
    };

    if (json.is_string())
    {
        addTarget(json.get<std::string>());
    }
    else if (json.is_array())
    {
        for (const auto& value : json)
        {
            addTarget(value.get<std::string>());
        }
    }
    else
    {
        throw std::runtime_error(
            "Animation target must be a string or array");
    }
    return target;
}

std::unique_ptr<Animation> Animation::fromJson(const nlohmann::json& json)
{
    const std::string type = json.at("type");
    const AnimationTarget target = targetFromJson(json.at("target"));

    if (type == "sin")
    {
        return std::make_unique<SinAnimation>(
            target,
            json.at("amplitudeX").get<float>(),
            json.at("amplitudeY").get<float>(),
            json.at("frequency").get<float>(),
            json.at("phase").get<float>()
        );
    }

    throw std::runtime_error(
        "Unknown animation type: " + type);
}

Transform Animator::animate(const Transform& transform, const std::vector<std::unique_ptr<Animation>>& animations, float time)
{
    Transform result = transform;
    for (const auto& animation : animations)
    {
        animation->apply(result, time);
    }
    return result;
}