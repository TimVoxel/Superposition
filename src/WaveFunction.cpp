#include <WaveFunction.hpp>
#include <cmath>
#include <utility>

WaveFunction::WaveFunction(
    float centerX,
    float centerY,
    float sigma
)
    : centerX_(centerX),
      centerY_(centerY),
      sigma_(sigma)
{
}

void WaveFunction::setCenter(float x, float y)
{
    centerX_ = x;
    centerY_ = y;
}

void WaveFunction::setSigma(float sigma)
{
    sigma_ = sigma;
}

std::pair<float, float> WaveFunction::sample(
    std::mt19937& generator
) const
{
    std::normal_distribution<float> distribution(
        0.0f,
        sigma_
    );

    return {
        centerX_ + distribution(generator),
        centerY_ + distribution(generator)
    };
}

float WaveFunction::probability(float x, float y) const
{
    const float dx = x - centerX_;
    const float dy = y - centerY_;

    const float exponent =
        -(dx * dx + dy * dy)
        / (2.0f * sigma_ * sigma_);

    return std::exp(exponent);
}