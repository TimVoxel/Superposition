#pragma once

#include <Particle.hpp>
#include <vector>
#include <random> 
#include <wavefunction/WaveFunction.hpp>
#include <ParticleSystemConfig.hpp>
#include <SceneObject.hpp>
#include <nlohmann/json.hpp>

class ParticleSystem : public SceneObject
{
public:
    explicit ParticleSystem(std::unique_ptr<WaveFunction> waveFunction, ParticleSystemConfig config, Transform transform = {{ 0.0f, 0.0f}, {1.0f, 1.0f}}, SceneObject* parent = nullptr)
        : SceneObject(transform, parent), waveFunction_(std::move(waveFunction)), config_(std::move(config))
    {
    }

    void spawn(float time);
    void spawn(float x, float y);
    void updateExisting(float deltaTime);
    void stop();
    const std::vector<Particle>& particles() const;
    const ParticleSystemConfig& config() const;
    const WaveFunction& waveFunction()
    {
        return *waveFunction_;
    }

    static std::unique_ptr<ParticleSystem> fromJson(const nlohmann::json& json);
private:
    std::vector<Particle> particles_;
    std::unique_ptr<WaveFunction> waveFunction_;
    ParticleSystemConfig config_;
    float spawnAccumulator_ = 0.0f;
    float spawnRatePS_ = 0.0f;
    float startTime_ = 0.0f;
    bool isActive_ = false;
    std::mt19937 generator_;
    std::uniform_real_distribution<float> phaseDistribution_{0.0f, 2.0f * 3.14159265f};

    void onStart(float currentTime) override;
    void onUpdate(float deltaTime, float currentTime) override;
};