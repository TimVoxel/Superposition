#pragma once

#include <ParticleSystem.hpp>
#include <WaveFunction.hpp>
#include <SceneObject.hpp>

class ParticleSystemLink : public SceneObject
{
public:
    ParticleSystemLink(ParticleSystem first, ParticleSystem second, WaveFunction waveFunction);

    void start(float currentTime) override;
    void update(float deltaTime, float currentTime) override;

    const ParticleSystem& first() const;
    const ParticleSystem& second() const;

private:
    ParticleSystem first_;
    ParticleSystem second_;
    WaveFunction waveFunction_;

    float startTime_ = 0.0f;
    bool isActive_ = false;

    float spawnRatePS_ = 0.0f;
    float spawnAccumulator_ = 0.0f;
};