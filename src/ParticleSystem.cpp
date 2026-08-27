#include <ParticleSystem.hpp>
#include <algorithm>
#include <iostream>

void ParticleSystem::onStart(float currentTime)
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
    Transform wt = worldTransform();
    x += wt.position.x;
    y += wt.position.y;
    particles_.push_back({
        x, y,
        0.0f,
        config_.lifetime,
        x, y,
        phase
    });
}

void ParticleSystem::onUpdate(float deltaTime, float currentTime)
{
    if (!isActive_)
    {
        return;
    }
    float elapsedTime = currentTime - startTime_;

    if (elapsedTime < config_.durationSeconds)
    {
        float spawnRateGrowthPS_ = config_.maxSpawnRatePS / config_.durationSeconds;
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
    Transform wt = worldTransform();
    float frequency = config_.frequency;
    float amplitudeX = config_.amplitude * wt.scale.x;
    float amplitudeY = config_.amplitude * wt.scale.y;

    for (Particle& particle : particles_)
    {
        particle.age += deltaTime;
        particle.x = particle.originX + std::sin(particle.age * frequency + particle.age) * amplitudeX;
        particle.y = particle.originY + std::cos(particle.age * frequency * 0.8f + particle.age) * amplitudeY;
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

std::unique_ptr<ParticleSystem> ParticleSystem::fromJson(const nlohmann::json& json)
{
    return std::make_unique<ParticleSystem>(json.at("config").get<ParticleSystemConfig>());
}