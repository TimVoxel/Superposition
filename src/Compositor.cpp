#include <Compositor.hpp>
#include <ParticleSystem.hpp>
#include <SceneObject.hpp>
#include <ParticleSystemLink.hpp>
#include <typeinfo>

Compositor::Compositor(const Scene& scene)
    : scene_(scene)
{
    size_t size = 0;
    for (const std::unique_ptr<SceneObject>& obj : scene_.objects())
    {
        if (const ParticleSystem* ps = dynamic_cast<const ParticleSystem*>(obj.get()))
        {
            size += ps->config().maxParticles;
        }
        else if (const ParticleSystemLink* l = dynamic_cast<const ParticleSystemLink*>(obj.get()))
        {
            size += l->first().config().maxParticles + l->second().config().maxParticles;
        }
        else
        {
            throw std::runtime_error(std::string("Unable to find the composition size of object of type: ") + typeid(*obj).name());
        }
    }   
    points_.reserve(size);
}

const std::vector<RenderPoint>& Compositor::compose()
{
    points_.clear();

    for (const std::unique_ptr<SceneObject>& obj : scene_.objects())
    {
        if (const ParticleSystem* ps = dynamic_cast<const ParticleSystem*>(obj.get()))
        {
            composeParticleSystem(*ps);
        }
        else if (const ParticleSystemLink* l = dynamic_cast<const ParticleSystemLink*>(obj.get()))
        {
            composeParticleSystem(l->first());
            composeParticleSystem(l->second());
        }
        else
        {
            throw std::runtime_error(std::string("Unable to compose scene object of type: ")+ typeid(*obj).name());
        }
    }
    return points_;
}

void Compositor::composeParticleSystem(const ParticleSystem& system)
{
    const auto& particles = system.particles();
    const auto& config = system.config();

    for (const Particle& particle : particles)
    {
        points_.push_back({
            .x = particle.x,
            .y = particle.y,
            .progress = particle.age / particle.lifetime,
            .pointSize = config.pointSize,
            .sizeIncrease = config.sizeIncrease,
            .r = config.color.r,
            .g = config.color.g,
            .b = config.color.b,
            .a = config.color.a
        });
    }
}