#pragma once

#include <animation/Animation.hpp>
#include <algorithm>
#include <cmath>

// Periodic on/off pulse with smoothed edges -- a repeating counterpart to
// EaseAnimation. Where EaseAnimation ramps once and holds, PulseAnimation
// swings back and forth between "off" (contributes 0) and "on" (contributes
// the full delta) every `period` seconds, snapping quickly between the two
// rather than smoothly breathing in and out like SinAnimation. Good for
// "this repeatedly toggles/collapses" effects -- a cloud that periodically
// collapses into a point and reforms, a blink, anything alternating.
//
// Internally: a sine wave at angular rate 2*pi/period is thresholded through
// a smoothstep across [-edge, edge] around each zero crossing, so it sits
// flat near 0 or 1 most of the time and transitions over roughly `edge`
// seconds around each crossing. `phase` shifts the wave in radians, same
// convention as SinAnimation, so a PulseAnimation can be phase-aligned with
// a SinAnimation driving some other object (e.g. make a collapse coincide
// with a pupil's sin-driven position hitting an extreme).
//
// Additive like EaseAnimation/SinAnimation: give the object a base value
// (e.g. "opacity": 0.0) and this contributes delta * signal(t) on top of it.
class PulseAnimation : public Animation
{
public:
    PulseAnimation(AnimationTarget target, float deltaX, float deltaY, float period, float phase, float edge)
        : Animation(target),
          deltaX_(deltaX),
          deltaY_(deltaY),
          period_(period),
          phase_(phase),
          edge_(std::clamp(edge, 0.001f, 0.999f))
    {
    }

    void apply(Transform& transform, float time) const override
    {
        const float twoPi = 6.28318530718f;
        const float raw = std::sin(twoPi * time / period_ + phase_);

        // map raw from [-edge, edge] -> [0, 1] and smoothstep it; outside
        // that band it's clamped flat at 0 or 1.
        const float t = std::clamp((raw + edge_) / (2.0f * edge_), 0.0f, 1.0f);
        const float signal = t * t * (3.0f - 2.0f * t);

        const float valueX = deltaX_ * signal;
        const float valueY = deltaY_ * signal;

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
    float period_;
    float phase_;
    float edge_;
};
