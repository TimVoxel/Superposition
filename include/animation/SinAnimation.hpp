#pragma once

#include <animation/Animation.hpp>
#include <cmath>

class SinAnimation : public Animation
{
public:
    SinAnimation(AnimationTarget target, float amplitudeX, float amplitudeY, float frequency, float phase)
        : Animation(target),
          amplitudeX_(amplitudeX),
          amplitudeY_(amplitudeY),
          frequency_(frequency),
          phase_(phase)
    {
    }

    void apply(Transform& transform, float time) const override
    {
        const float valueX = std::sin(time * frequency_ + phase_) * amplitudeX_;
        const float valueY = std::sin(time * frequency_ + phase_) * amplitudeY_;

        if (target() & AnimationTarget::Position)
        {
            transform.position.x += valueX;
            transform.position.y += valueY;
        }

        if (target() & AnimationTarget::Scale)
        {
            transform.scale.x += valueX;
            transform.scale.y += valueY;
        }
    }

private:
    float amplitudeX_;
    float amplitudeY_;
    float frequency_;
    float phase_;
};