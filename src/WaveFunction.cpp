#include <WaveFunction.hpp>
#include <cmath>
#include <utility>

WaveFunction::WaveFunction(float sigma)
{
    setSigma(sigma); 
}

std::pair<float, float> WaveFunction::sample()
{
    return {
        distribution_(generator_),
        distribution_(generator_)
    };
}

float WaveFunction::phase()
{
    return phaseDistribution_(generator_);
}

void WaveFunction::setSigma(float sigma)
{
    sigma_ = sigma;
    distribution_ = std::normal_distribution<float>(0.0f, sigma_);
}

void from_json(const nlohmann::json& json, WaveFunction& waveFunction)
{
    waveFunction.setSigma(json["sigma"].get<float>());
}