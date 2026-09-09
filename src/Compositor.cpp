#include <Compositor.hpp>
#include <ParticleSystem.hpp>
#include <SceneObject.hpp>
#include <ParticleSystemLink.hpp>
#include <typeinfo>
#include <Container.hpp>
#include <iostream>

size_t getSizeOf(const SceneObject& obj)
{
    size_t size = 0;
    if (const auto* ps = dynamic_cast<const ParticleSystem*>(&obj))
    {
        size += ps->config().maxParticles;
    }
    for (const auto& child : obj.children())
    {
        size += getSizeOf(*child);
    }
    return size;
}

Compositor::Compositor(const Scene& scene) : scene_(scene)
{
    size_t size = 0;
    for (const auto& obj : scene_.objects())
    {
        size += getSizeOf(*obj);
    }
    points_.reserve(size);
}

const std::vector<RenderPoint>& Compositor::compose()
{
    points_.clear();

    for (const std::unique_ptr<SceneObject>& obj : scene_.objects())
    {
        composeObject(*obj);
    }
    return points_;
}

void Compositor::composeObject(const SceneObject& obj)
{
    if (auto* ps = dynamic_cast<const ParticleSystem*>(&obj))
    {
        composeParticleSystem(*ps);
    }
    for (const auto& child : obj.children())
    {
        composeObject(*child);
    }
}

void Compositor::composeParticleSystem(const ParticleSystem& system)
{
    const auto& particles = system.particles();
    const auto& config = system.config();
    const auto& transform = system.worldTransform();

    float size = config.pointSize * transform.scale.x;
    float sizeIncrease = config.sizeIncrease * transform.scale.x;
    float tx = transform.position.x;
    float ty = transform.position.y;

    for (const Particle& particle : particles)
    {
        points_.push_back({
            .x = tx + particle.x * transform.scale.x,
            .y = ty + particle.y * transform.scale.y,
            .progress = particle.age / particle.lifetime,
            .pointSize = size,
            .sizeIncrease = sizeIncrease,
            .r = config.color.r,
            .g = config.color.g,
            .b = config.color.b,
            .a = config.color.a * transform.opacity
        });
    }
}