#pragma once

#include <wavefunction/WaveFunction.hpp>

#include <complex>
#include <random>
#include <vector>

class HydrogenWaveFunction : public WaveFunction
{
public:
    HydrogenWaveFunction(
        int resolution,
        float size,
        float a0,
        float timeStep = 0.0001f
    );
    
    HydrogenWaveFunction(
        const std::string& orbitalPath,
        float timeStep = 0.0001f
    );

    void update(float deltaTime) override;
    std::pair<float, float> sample() override;

private:
    using Complex = std::complex<float>;

    const float pi = 3.14159265359f;
    const float hbar = 1.0f;
    const float mass = 1.0f;
    const float k = 1.0f;
    const float epsilon = 0.01f;

    int resolution_;
    int resolutionNoWalls_;
    float size_;
    float dx_;
    float a0_;
    float timeStep_;
    const float UPDATE_HZ = 5; 
    float cachedDeltaTime_ = -1.0f; 
    Complex alpha_;
    
    float accumulator_ = 0.0f;

    std::vector<Complex> psi_;
    std::vector<Complex> nextPsi_;
    std::vector<Complex> halfStep_;

    std::vector<Complex> subdiag_;
    std::vector<Complex> supdiag_;
    std::vector<Complex> diagBuf_;
    std::vector<Complex> rhsBuf_;
    std::vector<Complex> solved_;
    std::vector<Complex> cPrime_;
    std::vector<Complex> dPrime_;

    inline void solveTridiagonal();

    std::mt19937 generator_{std::random_device{}()};
    std::uniform_real_distribution<float> uniformDistribution_{0.0f, 1.0f};

    void normalize();
    inline int index(int x, int y) const;
    void initialize();
    void step(float deltaTime);
    void allocateBuffers();
};