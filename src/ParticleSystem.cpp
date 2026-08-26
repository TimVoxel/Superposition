#include <ParticleSystem.hpp>
#include <algorithm>

ParticleSystem::ParticleSystem(float durationSeconds, float maxSpawnRatePS)
    : maxSpawnRatePS_(maxSpawnRatePS)
{
    spawnRateGrowthPS_ = maxSpawnRatePS / durationSeconds;
}

void ParticleSystem::spawn()
{
    auto [x, y] = waveFunction_.sample(generator_);
    particles_.push_back({
        x, y,
        0.0f,
        5.0f,
        x, y,
        phaseDistribution_(generator_)
    });
}
void ParticleSystem::update(float deltaTime, bool shouldSpawn)
{
    spawnRatePS_ = std::min(spawnRatePS_ + spawnRateGrowthPS_ * deltaTime, maxSpawnRatePS_);
    spawnAccumulator_ += deltaTime * spawnRatePS_;

    while (shouldSpawn && spawnAccumulator_ >= 1.0f)
    {
        spawn();
        spawnAccumulator_ -= 1.0f;
    }

    for (Particle& particle : particles_)
    {
        particle.age += deltaTime;
        particle.x = particle.originX + std::sin(particle.age * frequency + particle.age) * amplitude;
        particle.y = particle.originY + std::cos(particle.age * frequency * 0.8f + particle.age) * amplitude;
    }
    

    particles_.erase(
        std::remove_if(
            particles_.begin(),
            particles_.end(),
            [](const Particle& particle)
            {
                return particle.age >= particle.lifetime;
            }
        ),
        particles_.end()
    );
}

const std::vector<Particle>& ParticleSystem::particles() const
{
    return particles_;
}