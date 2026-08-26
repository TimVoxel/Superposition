#pragma once

#include <random>

class WaveFunction
{
public:
    WaveFunction(
        float centerX,
        float centerY,
        float sigma
    );

    void setCenter(float x, float y);
    void setSigma(float sigma);

    std::pair<float, float> sample(std::mt19937& generator) const;

    float probability(float x, float y) const;

private:
    float centerX_;
    float centerY_;
    float sigma_;
};