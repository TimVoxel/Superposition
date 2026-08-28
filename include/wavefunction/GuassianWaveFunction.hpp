#include <wavefunction/WaveFunction.hpp>

#include <random>

class GaussianWaveFunction : public WaveFunction
{
public:
    explicit GaussianWaveFunction(float sigma)
        : sigma_(sigma),
          distribution_(0.0f, sigma)
    {
    }

    std::pair<float, float> sample(float time) override
    {
        return {
            distribution_(generator_),
            distribution_(generator_)
        };
    }

private:
    float sigma_;
    std::mt19937 generator_;
    std::normal_distribution<float> distribution_;
};