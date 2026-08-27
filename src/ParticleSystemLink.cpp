#include <ParticleSystemLink.hpp>

ParticleSystemLink::ParticleSystemLink(ParticleSystem first, ParticleSystem second, WaveFunction waveFunction)
    : first_(std::move(first)), second_(std::move(second)), waveFunction_(std::move(waveFunction))
{
}

void ParticleSystemLink::start(float currentTime)
{
    startTime_ = currentTime;
    isActive_ = true;
    spawnRatePS_ = 0.0f;
    spawnAccumulator_ = 0.0f;
    first_.start(currentTime);
    second_.start(currentTime);
}

void ParticleSystemLink::update(float deltaTime, float currentTime)
{
    if (!isActive_)
    {
        return;
    }

    const float elapsedTime = currentTime - startTime_;

    const ParticleSystemConfig& firstConfig = first_.config();
    const ParticleSystemConfig& secondConfig = second_.config();

    if (elapsedTime < firstConfig.durationSeconds)
    {
        spawnRatePS_ = std::min(
            spawnRatePS_ +
            (firstConfig.maxSpawnRatePS / firstConfig.durationSeconds) * deltaTime,
            firstConfig.maxSpawnRatePS
        );

        spawnAccumulator_ += deltaTime * spawnRatePS_;

        while (spawnAccumulator_ >= 1.0f)
        {
            auto [x, y] = waveFunction_.sample();
            float phase = waveFunction_.phase();
            first_.spawn(firstConfig.centerPos.x + x, firstConfig.centerPos.y + y, phase);
            second_.spawn(secondConfig.centerPos.x - x, secondConfig.centerPos.y - y, 1 - phase);
            spawnAccumulator_ -= 1.0f;
        }
    }

    first_.updateExisting(deltaTime);
    second_.updateExisting(deltaTime);
}

const ParticleSystem& ParticleSystemLink::first() const { return first_; }
const ParticleSystem& ParticleSystemLink::second() const { return second_; }