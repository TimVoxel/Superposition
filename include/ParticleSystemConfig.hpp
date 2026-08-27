#pragma once

#include <Color.hpp>
#include <nlohmann/json.hpp>
#include <Math.hpp>
#include <WaveFunction.hpp>
#include <optional>

struct ParticleSystemConfig
{
    Vec2f centerPos;
    float durationSeconds;
    float fadeoutSeconds;
    float maxSpawnRatePS;
    int maxParticles;
    float lifetime;
    float pointSize;
    float sizeIncrease;
    float amplitude;
    float frequency;
    Color color;
    std::optional<WaveFunction> waveFunction;
};

void from_json(const nlohmann::json& json, ParticleSystemConfig& config);