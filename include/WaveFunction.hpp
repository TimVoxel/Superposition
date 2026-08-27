#pragma once

#include <random>
#include <nlohmann/json.hpp>

class WaveFunction
{
public:
    WaveFunction(float sigma = 1.0f);
    std::pair<float, float> sample();
    float phase();
    void setSigma(float s);

private:
    float sigma_;
    std::mt19937 generator_{std::random_device{}()};
    std::normal_distribution<float> distribution_;
    std::uniform_real_distribution<float> phaseDistribution_{0, 1};
};

void from_json(const nlohmann::json& json, WaveFunction& waveFunction);