#pragma once

#include <RenderPoint.hpp>
#include <Scene.hpp>

#include <vector>
#include <ParticleSystem.hpp>

class Compositor
{
public:
    explicit Compositor(const Scene& scene);
    const std::vector<RenderPoint>& compose();
private:
    const Scene& scene_;
    std::vector<RenderPoint> points_;

    void composeObject(const SceneObject& obj);
    void composeParticleSystem(const ParticleSystem& particleSystem);
};