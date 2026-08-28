#include <ParticleSystem.hpp>
#include <algorithm>
#include <iostream>

void ParticleSystem::onStart(float currentTime)
{
    startTime_ = currentTime;
    isActive_ = true;
    spawnRatePS_ = 0.0f;
    spawnAccumulator_ = 1.0f;
}

void ParticleSystem::stop()
{
    isActive_ = false;
}

void ParticleSystem::spawn(float time)
{
    if (particles_.size() >= config_.maxParticles)
    {
        return;
    }
    if (waveFunction_ == nullptr)
    {
        throw std::runtime_error("Cannot randomly spawn particles inside a particle system without a wave function");
        return;
    }
    auto [x, y] = waveFunction_->sample(time);
    spawn(x, y);
}

void ParticleSystem::spawn(float x, float y)
{
    if (particles_.size() >= config_.maxParticles)
    {
        return;
    }
    particles_.push_back({
        x, y,
        0.0f,
        config_.lifetime,
        x, y,
        phaseDistribution_(generator_)
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
            spawn(currentTime);
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
    ParticleSystemConfig config = json.at("config").get<ParticleSystemConfig>();
    std::unique_ptr<WaveFunction> waveFunction;

    if (json.contains("waveFunction"))
    {
        waveFunction = WaveFunction::fromJson(json.at("waveFunction"));
    }
    return std::make_unique<ParticleSystem>(std::move(waveFunction), config);
}