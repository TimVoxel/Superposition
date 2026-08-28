#pragma once

#include <Transform.hpp>
#include <memory>
#include <vector>
#include <nlohmann/json.hpp>

enum class AnimationTarget
{
    None     = 0,
    Position = 1 << 0,
    Scale    = 1 << 1
};

constexpr AnimationTarget operator|(AnimationTarget a, AnimationTarget b)
{
    return static_cast<AnimationTarget>(
        static_cast<int>(a) | static_cast<int>(b)
    );
}

constexpr bool operator&(AnimationTarget a, AnimationTarget b)
{
    return (static_cast<int>(a) & static_cast<int>(b)) != 0;
}

class Animation
{
public:
    explicit Animation(AnimationTarget target)
        : target_(target) {}

    virtual ~Animation() = default;

    AnimationTarget target() const
    {
        return target_;
    }

    virtual void apply(Transform& transform, float time) const = 0;
    static std::unique_ptr<Animation> fromJson(const nlohmann::json& json);

private:
    AnimationTarget target_;
};

class Animator
{
public:
    static Transform animate(const Transform& transform, const std::vector<std::unique_ptr<Animation>>& animations, float time);
};