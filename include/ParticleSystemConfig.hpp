#pragma once

#include <Color.hpp>
#include <nlohmann/json.hpp>
#include <Math.hpp>
#include <wavefunction/WaveFunction.hpp>
#include <optional>

struct ParticleSystemConfig
{
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
};

void from_json(const nlohmann::json& json, ParticleSystemConfig& config);