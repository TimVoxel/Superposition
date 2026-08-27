#pragma once

#include <Particle.hpp>
#include <vector>
#include <random> 
#include <WaveFunction.hpp>
#include <ParticleSystemConfig.hpp>
#include <SceneObject.hpp>

class ParticleSystem : public SceneObject
{
public:
    explicit ParticleSystem(ParticleSystemConfig config);
    void spawn();
    void spawn(float x, float y, float phase);
    void start(float currentTime) override;
    void update(float deltaTime, float currentTime) override;
    void updateExisting(float deltaTime);
    void stop();
    const std::vector<Particle>& particles() const;
    const ParticleSystemConfig& config() const;
private:
    std::vector<Particle> particles_;
    ParticleSystemConfig config_;
    float spawnAccumulator_ = 0.0f;
    float spawnRatePS_ = 0.0f;
    float spawnRateGrowthPS_ = 3.0f;
    float startTime_ = 0.0f;
    bool isActive_ = false;
};