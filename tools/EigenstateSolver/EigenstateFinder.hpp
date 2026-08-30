#pragma once

#include <complex>
#include <random>
#include <vector>

class EigenstateFinder
{
public:
    using Complex = std::complex<float>;

    EigenstateFinder(int resolution, float size, float a0);

    struct Result
    {
        std::vector<Complex> psi;
        float energy;
        int m;
        int radialLevel;
    };

    Result findNext(int m, int iterations, float dtau);

private:
    int resolution_;
    int resolutionNoWalls_;
    float size_;
    float a0_;
    float dx_;

    static constexpr float hbar = 1.0f;
    static constexpr float mass = 1.0f;
    static constexpr float k = 1.0f;
    static constexpr float epsilon = 0.01f;

    std::vector<std::pair<int, std::vector<Complex>>> foundStates_;

    std::vector<Complex> halfStep_;
    std::vector<Complex> nextPsi_;
    std::vector<Complex> subdiag_;
    std::vector<Complex> supdiag_;
    std::vector<Complex> diagBuf_;
    std::vector<Complex> rhsBuf_;
    std::vector<Complex> solved_;
    std::vector<Complex> cPrime_;
    std::vector<Complex> dPrime_;

    std::mt19937 generator_{std::random_device{}()};
    std::uniform_real_distribution<float> uniformDistribution_{0.0f, 1.0f};

    int index(int x, int y) const;
    float potentialAt(int x, int y) const;

    std::vector<Complex> seedWithAngularMomentum(int m);
    void stepImaginary(float dtau, std::vector<Complex>& state);
    void solveTridiagonal();
    void normalizeBuffer(std::vector<Complex>& buf) const;
    Complex innerProduct(const std::vector<Complex>& a, const std::vector<Complex>& b) const;
    void projectOut(std::vector<Complex>& psi, const std::vector<Complex>& basisState) const;
    float estimateEnergy(const std::vector<Complex>& state) const;
};