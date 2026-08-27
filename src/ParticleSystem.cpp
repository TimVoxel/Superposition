#include <ParticleSystem.hpp>
#include <algorithm>

ParticleSystem::ParticleSystem(ParticleSystemConfig config)
    : config_(config)
{
    spawnRateGrowthPS_ = config.maxSpawnRatePS / config.durationSeconds;
}

void ParticleSystem::start(float currentTime)
{
    startTime_ = currentTime;
    isActive_ = true;
    spawnRatePS_ = 0.0f;
    spawnAccumulator_ = 0.0f;
}

void ParticleSystem::stop()
{
    isActive_ = false;
}

void ParticleSystem::spawn()
{
    if (!config_.waveFunction.has_value())
    {
        throw std::runtime_error("Cannot randomly spawn particles inside a particle system without a wave function");
        return;
    }
    WaveFunction& waveFunction = config_.waveFunction.value();
    auto [x, y] = waveFunction.sample();
    float phase = waveFunction.phase(); 
    spawn(x, y, phase);
}

void ParticleSystem::spawn(float x, float y, float phase)
{
    particles_.push_back({
        config_.centerPos.x + x, config_.centerPos.y + y,
        0.0f,
        config_.lifetime,
        x, y,
        phase
    });
}

void ParticleSystem::update(float deltaTime, float currentTime)
{
    if (!isActive_)
    {
        return;
    }
    float elapsedTime = currentTime - startTime_;

    if (elapsedTime < config_.durationSeconds)
    {
        spawnRatePS_ = std::min(spawnRatePS_ + spawnRateGrowthPS_ * deltaTime, config_.maxSpawnRatePS);
        spawnAccumulator_ += deltaTime * spawnRatePS_;

        while (spawnAccumulator_ >= 1.0f)
        {
            spawn();
            spawnAccumulator_ -= 1.0f;
        }
    }
    updateExisting(deltaTime);
}

void ParticleSystem::updateExisting(float deltaTime)
{
    float frequency = config_.frequency;
    float amplitude = config_.amplitude;

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

const ParticleSystemConfig& ParticleSystem::config() const { return config_; }