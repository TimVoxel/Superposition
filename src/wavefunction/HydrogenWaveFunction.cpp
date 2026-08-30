#include <wavefunction/HydrogenWaveFunction.hpp>

#include <cmath>
#include <stdexcept>
#include <algorithm>
#include <limits>
#include <iostream>
#include <wavefunction/OrbitalFile.hpp>

HydrogenWaveFunction::HydrogenWaveFunction(int resolution, float size, float a0, float timeStep)
    : resolution_(resolution),
      resolutionNoWalls_(resolution - 2),
      size_(size),
      dx_(size / resolution),
      a0_(a0),
      timeStep_(timeStep),
      psi_(resolution * resolution),
      nextPsi_(resolution * resolution),
      halfStep_(resolution * resolution),
      alpha_(0.0f, timeStep * hbar / (4.0f * mass * dx_ * dx_))
{
    if (resolution < 3)
    {
        throw std::invalid_argument("Hydrogen wave function resolution must be at least 3");
    }
    if (size <= 0.0f)
    {
        throw std::invalid_argument("Hydrogen wave function size must be positive");
    }
    if (a0 <= 0.0f)
    {
        throw std::invalid_argument("Hydrogen Bohr radius must be positive");
    }
    if (timeStep <= 0.0f)
    {
        throw std::invalid_argument("Hydrogen wave function time step must be positive");
    }

    allocateBuffers();
    initialize();
}

HydrogenWaveFunction::HydrogenWaveFunction(const std::string& orbitalPath, float timeStep)
    : timeStep_(timeStep)
{
    if (timeStep <= 0.0f)
    {
        throw std::invalid_argument("Hydrogen wave function time step must be positive");
    }

    OrbitalData data = OrbitalFile::read(orbitalPath);

    if (data.resolution < 3)
    {
        throw std::invalid_argument("Orbital file has invalid resolution");
    }

    resolution_ = data.resolution;
    resolutionNoWalls_ = resolution_ - 2;
    size_ = data.size;
    dx_ = size_ / resolution_;
    a0_ = data.a0;

    psi_ = std::move(data.psi);
    nextPsi_.resize(resolution_ * resolution_);
    halfStep_.resize(resolution_ * resolution_);

    alpha_ = Complex(0.0f, timeStep_ * hbar / (4.0f * mass * dx_ * dx_));

    allocateBuffers();
    normalize();

    std::cout << "Loaded orbital from " << orbitalPath
               << " (energy = " << data.energy << ")\n";
}

void HydrogenWaveFunction::allocateBuffers()
{
    subdiag_.resize(resolutionNoWalls_);
    supdiag_.resize(resolutionNoWalls_);
    diagBuf_.resize(resolutionNoWalls_);
    rhsBuf_.resize(resolutionNoWalls_);
    solved_.resize(resolutionNoWalls_);
    cPrime_.resize(resolutionNoWalls_);
    dPrime_.resize(resolutionNoWalls_);
}

int HydrogenWaveFunction::index(int x, int y) const
{
    return y * resolution_ + x;
}

void HydrogenWaveFunction::initialize()
{
    float normalization = 1.0f / std::sqrt(pi * a0_ * a0_);

    for (int y = 0; y < resolution_; ++y)
    {
        for (int x = 0; x < resolution_; ++x)
        {
            float worldX = (static_cast<float>(x) + 0.5f) * dx_ - size_ * 0.5f;
            float worldY = (static_cast<float>(y) + 0.5f) * dx_ - size_ * 0.5f;
            float r = std::sqrt(worldX * worldX + worldY * worldY);
            float amplitude = normalization * std::exp(-r / a0_);
            psi_[index(x, y)] = Complex(amplitude, 0.0f);
        }
    }
}

void HydrogenWaveFunction::update(float deltaTime)
{
    // Go ahead and uncomment this if you want your wave function to change over time according to the Schrödinger equation
    // However, in the case of the hydrogen wave function all this will do is rotate it in the imaginary plane
    // That's not exactly the most exciting visual (the plane is imaginary for a reason)
    // Uncommenting this also makes the fps drop 10 times for any resolution higher than 64.
    // Exciting times we live in

    // accumulator_ += deltaTime;
    // const float updateInterval = 1.0f / UPDATE_HZ;

    // while (accumulator_ >= updateInterval)
    // {
    //     float remaining = updateInterval;

    //     while (remaining > 0.0f)
    //     {
    //         float stepSize = std::min(remaining, timeStep_);
    //         step(stepSize);
    //         remaining -= stepSize;
    //     }
    //     accumulator_ -= updateInterval;
    // }
}

void HydrogenWaveFunction::step(float deltaTime)
{
    const int n = resolution_ - 2; // walls are fixed at 0

    const Complex alpha = deltaTime == cachedDeltaTime_ 
            ? alpha_
            : Complex(0.0f, deltaTime * hbar / (4.0f * mass * dx_ * dx_));

    if (deltaTime != cachedDeltaTime_)
    {
        subdiag_.assign(n, -alpha);
        supdiag_.assign(n, -alpha);
        cachedDeltaTime_ = deltaTime;
    }
    
    auto potentialAt = [&](int x, int y) -> float
    {
        float worldX = (static_cast<float>(x) + 0.5f) * dx_ - size_ * 0.5f;
        float worldY = (static_cast<float>(y) + 0.5f) * dx_ - size_ * 0.5f;
        float r = std::sqrt(worldX * worldX + worldY * worldY + epsilon * epsilon);
        return -k / r;
    };

    // half-step 1: implicit in x, explicit in y
    for (int y = 1; y < resolution_ - 1; ++y)
    {
        for (int xi = 0; xi < n; ++xi)
        {
            int x = xi + 1;
            Complex p(0.0f, deltaTime * potentialAt(x, y) / (4.0f * hbar));
            diagBuf_[xi] = 1.0f + 2.0f * alpha + p;
            Complex yNeighbors = alpha * (psi_[index(x, y + 1)] - 2.0f * psi_[index(x, y)] + psi_[index(x, y - 1)]);
            rhsBuf_[xi] = psi_[index(x, y)] + yNeighbors - p * psi_[index(x, y)];
        }

        solveTridiagonal();

        for (int xi = 0; xi < n; ++xi)
        {
            halfStep_[index(xi + 1, y)] = solved_[xi];
        }
    }

    for (int x = 0; x < resolution_; ++x)
    {
        halfStep_[index(x, 0)] = Complex(0.0f, 0.0f);
        halfStep_[index(x, resolution_ - 1)] = Complex(0.0f, 0.0f);
    }
    for (int y = 0; y < resolution_; ++y)
    {
        halfStep_[index(0, y)] = Complex(0.0f, 0.0f);
        halfStep_[index(resolution_ - 1, y)] = Complex(0.0f, 0.0f);
    }

    // half-step 2: implicit in y, explicit in x
    for (int x = 1; x < resolution_ - 1; ++x)
    {
        for (int yi = 0; yi < n; ++yi)
        {
            int y = yi + 1;
            Complex p(0.0f, deltaTime * potentialAt(x, y) / (4.0f * hbar));
            diagBuf_[yi] = 1.0f + 2.0f * alpha + p;
            Complex xNeighbors = alpha * (halfStep_[index(x + 1, y)] - 2.0f * halfStep_[index(x, y)] + halfStep_[index(x - 1, y)]);
            rhsBuf_[yi] = halfStep_[index(x, y)] + xNeighbors - p * halfStep_[index(x, y)];
        }

        solveTridiagonal();

        for (int yi = 0; yi < n; ++yi)
        {
            nextPsi_[index(x, yi + 1)] = solved_[yi];
        }
    }

    for (int x = 0; x < resolution_; ++x)
    {
        nextPsi_[index(x, 0)] = Complex(0.0f, 0.0f);
        nextPsi_[index(x, resolution_ - 1)] = Complex(0.0f, 0.0f);
    }
    for (int y = 0; y < resolution_; ++y)
    {
        nextPsi_[index(0, y)] = Complex(0.0f, 0.0f);
        nextPsi_[index(resolution_ - 1, y)] = Complex(0.0f, 0.0f);
    }

    psi_.swap(nextPsi_);
    normalize();
}

void HydrogenWaveFunction::normalize()
{
    float totalProbability = 0.0f;

    for (const auto& value : psi_)
    {
        totalProbability += std::norm(value);
    }
    if (totalProbability <= 0.0f)
    {
        return;
    }

    const float norm = 1.0f / std::sqrt(totalProbability * dx_ * dx_);
    for (auto& value : psi_)
    {
        value *= norm;
    }
}

std::pair<float, float> HydrogenWaveFunction::sample()
{
    float totalProbability = 0.0f;

    for (const auto& value : psi_)
    {
        totalProbability += std::norm(value);
    }
    if (totalProbability <= 0.0f)
    {
        return {0.0f, 0.0f};
    }

    float target = uniformDistribution_(generator_) * totalProbability;
    float accumulated = 0.0f;

    for (int y = 0; y < resolution_; ++y)
    {
        for (int x = 0; x < resolution_; ++x)
        {
            accumulated += std::norm(psi_[index(x, y)]);

            if (accumulated >= target)
            {
                float worldX = (static_cast<float>(x) + 0.5f) * dx_ - size_ * 0.5f;
                float worldY = (static_cast<float>(y) + 0.5f) * dx_ - size_ * 0.5f;
                float ndcX = worldX / (size_ * 0.5f);
                float ndcY = worldY / (size_ * 0.5f);

                return {ndcX, ndcY};
            }
        }
    }

    return {0.0f, 0.0f};
}

void HydrogenWaveFunction::solveTridiagonal()
{
    cPrime_[0] = subdiag_[0] / diagBuf_[0];
    dPrime_[0] = rhsBuf_[0] / diagBuf_[0];

    for (int idx = 1; idx < resolutionNoWalls_; ++idx)
    {
        Complex denom = diagBuf_[idx] - subdiag_[idx] * cPrime_[idx - 1];
        cPrime_[idx] = supdiag_[idx] / denom;
        dPrime_[idx] = (rhsBuf_[idx] - subdiag_[idx] * dPrime_[idx - 1]) / denom;
    }

    solved_[resolutionNoWalls_ - 1] = dPrime_[resolutionNoWalls_ - 1];
    for (int idx = resolutionNoWalls_ - 2; idx >= 0; --idx)
    {
        solved_[idx] = dPrime_[idx] - cPrime_[idx] * solved_[idx + 1];
    }
}