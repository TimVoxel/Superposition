#include <wavefunction/WaveFunction.hpp>

#include <random>

//this is not actually a wave function but idk calling it a distribution is inconsistent

class GaussianWaveFunction : public WaveFunction
{
public:
    explicit GaussianWaveFunction(float sigma)
        : sigma_(sigma),
          distribution_(0.0f, sigma)
    {
    }

    void update(float deltaTime) override {};

    std::pair<float, float> sample() override
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