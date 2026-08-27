#pragma once

#include <Particle.hpp>
#include <vector>
#include <random> 
#include <WaveFunction.hpp>
#include <ParticleSystemConfig.hpp>
#include <SceneObject.hpp>
#include <nlohmann/json.hpp>

class ParticleSystem : public SceneObject
{
public:
    explicit ParticleSystem(ParticleSystemConfig config, Transform transform = {{ 0.0f, 0.0f}, {1.0f, 1.0f}}, SceneObject* parent = nullptr)
        : SceneObject(transform, parent), config_(std::move(config))
    {
    }

    void spawn();
    void spawn(float x, float y, float phase);
    void updateExisting(float deltaTime);
    void stop();
    const std::vector<Particle>& particles() const;
    const ParticleSystemConfig& config() const;

    static std::unique_ptr<ParticleSystem> fromJson(const nlohmann::json& json);
private:
    std::vector<Particle> particles_;
    ParticleSystemConfig config_;
    float spawnAccumulator_ = 0.0f;
    float spawnRatePS_ = 0.0f;
    float startTime_ = 0.0f;
    bool isActive_ = false;

    void onStart(float currentTime) override;
    void onUpdate(float deltaTime, float currentTime) override;
};