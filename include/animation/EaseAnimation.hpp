#pragma once

#include <animation/Animation.hpp>
#include <algorithm>

// One-shot, non-periodic transition: ramps from 0 up to (deltaX, deltaY) over
// [startTime, startTime + duration] using a smoothstep curve, then holds at
// the full delta forever after (and contributes nothing before startTime).
//
// Additive, like SinAnimation -- it ADDS its ramped delta on top of the base
// transform value (and on top of whatever other animations on the same
// object already contributed) rather than overwriting it outright. That's
// what lets several EaseAnimations targeting the same property compose
// correctly in sequence (e.g. one ramping opacity 0 -> 1 early on, another
// ramping it back down to 0 near the end): each only contributes within its
// own window, so they stack instead of one clobbering the other's hold value.
// To get a one-shot fade from A to B, set the object's own base transform to
// A (e.g. "opacity": 0.0) and give this animation a delta of (B - A).
//
// "time" is elapsed time since this object's own animations started
// (SceneObject::applyAnimation tracks that per object), not raw wall-clock
// time, so startTime/duration are always relative to when the scene/object
// began -- deterministic regardless of program startup latency.
class EaseAnimation : public Animation
{
public:
    EaseAnimation(AnimationTarget target, float deltaX, float deltaY, float startTime, float duration)
        : Animation(target),
          deltaX_(deltaX),
          deltaY_(deltaY),
          startTime_(startTime),
          duration_(duration)
    {
    }

    void apply(Transform& transform, float time) const override
    {
        float t = duration_ > 0.0f
            ? std::clamp((time - startTime_) / duration_, 0.0f, 1.0f)
            : (time >= startTime_ ? 1.0f : 0.0f);

        const float smooth = t * t * (3.0f - 2.0f * t); // smoothstep

        const float valueX = deltaX_ * smooth;
        const float valueY = deltaY_ * smooth;

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

        if (target() & AnimationTarget::Opacity)
        {
            transform.opacity += valueX; // opacity is scalar; only X is used
        }
    }

private:
    float deltaX_;
    float deltaY_;
    float startTime_;
    float duration_;
};
