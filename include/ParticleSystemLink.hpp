#pragma once

#include <ParticleSystem.hpp>
#include <wavefunction/WaveFunction.hpp>
#include <SceneObject.hpp>
#include <nlohmann/json.hpp>

class ParticleSystemLink : public SceneObject
{
public:
    ParticleSystemLink();
    ParticleSystemLink(
        std::unique_ptr<ParticleSystem> first,
        std::unique_ptr<ParticleSystem> second, 
        std::unique_ptr<WaveFunction> waveFunction,
        Transform transform = { {0.0f, 0.0f}, {1.0f, 1.0f} }, SceneObject* parent = nullptr)
            : SceneObject(transform, parent), waveFunction_(std::move(waveFunction))
    {
        first_ = first.get();
        second_ = second.get();
        addChild(std::move(first));
        addChild(std::move(second));
    }

    void start(float currentTime) override;
    void update(float deltaTime, float currentTime) override;

    const ParticleSystem* first() const;
    const ParticleSystem* second() const;

    static std::unique_ptr<ParticleSystemLink> fromJson(const nlohmann::json& json);

private:
    ParticleSystem* first_;
    ParticleSystem* second_;
    std::unique_ptr<WaveFunction> waveFunction_;

    float startTime_ = 0.0f;
    bool isActive_ = false;
    float spawnRatePS_ = 0.0f;
    float spawnAccumulator_ = 0.0f;

    void onStart(float currentTime) override;
    void onUpdate(float deltaTime, float currentTime) override;
};
