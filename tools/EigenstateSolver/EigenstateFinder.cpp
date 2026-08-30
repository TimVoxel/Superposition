#include "EigenstateFinder.hpp"

#include <cmath>

// This models a 2d hydrogen so don't get confused if the probability cloud doesn't look like the ones you are familiar with

EigenstateFinder::EigenstateFinder(int resolution, float size, float a0)
    : resolution_(resolution),
      resolutionNoWalls_(resolution - 2),
      size_(size),
      a0_(a0),
      dx_(size / resolution)
{
    halfStep_.resize(resolution_ * resolution_);
    nextPsi_.resize(resolution_ * resolution_);
    subdiag_.resize(resolutionNoWalls_);
    supdiag_.resize(resolutionNoWalls_);
    diagBuf_.resize(resolutionNoWalls_);
    rhsBuf_.resize(resolutionNoWalls_);
    solved_.resize(resolutionNoWalls_);
    cPrime_.resize(resolutionNoWalls_);
    dPrime_.resize(resolutionNoWalls_);
}

int EigenstateFinder::index(int x, int y) const
{
    return y * resolution_ + x;
}

float EigenstateFinder::potentialAt(int x, int y) const
{
    float worldX = (static_cast<float>(x) + 0.5f) * dx_ - size_ * 0.5f;
    float worldY = (static_cast<float>(y) + 0.5f) * dx_ - size_ * 0.5f;
    float r = std::sqrt(worldX * worldX + worldY * worldY + epsilon * epsilon);
    return -k / r;
}

void EigenstateFinder::normalizeBuffer(std::vector<Complex>& buf) const
{
    float totalProbability = 0.0f;
    for (const auto& value : buf)
    {
        totalProbability += std::norm(value);
    }
    if (totalProbability <= 0.0f)
    {
        return;
    }
    const float scale = 1.0f / std::sqrt(totalProbability * dx_ * dx_);
    for (auto& value : buf)
    {
        value *= scale;
    }
}

EigenstateFinder::Complex EigenstateFinder::innerProduct(
    const std::vector<Complex>& a,
    const std::vector<Complex>& b) const
{
    Complex sum(0.0f, 0.0f);
    for (std::size_t i = 0; i < a.size(); ++i)
    {
        sum += std::conj(a[i]) * b[i];
    }
    return sum * (dx_ * dx_);
}

void EigenstateFinder::projectOut(
    std::vector<Complex>& psi,
    const std::vector<Complex>& basisState) const
{
    Complex overlap = innerProduct(basisState, psi);
    for (std::size_t i = 0; i < psi.size(); ++i)
    {
        psi[i] -= overlap * basisState[i];
    }
}

void EigenstateFinder::solveTridiagonal()
{
    cPrime_[0] = supdiag_[0] / diagBuf_[0];
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

void EigenstateFinder::stepImaginary(float dtau, std::vector<Complex>& state)
{
    const int n = resolutionNoWalls_;

    const Complex alpha(dtau * hbar / (4.0f * mass * dx_ * dx_), 0.0f);
    subdiag_.assign(n, -alpha);
    supdiag_.assign(n, -alpha);

    // half-step 1: implicit in x, explicit in y
    for (int y = 1; y < resolution_ - 1; ++y)
    {
        for (int xi = 0; xi < n; ++xi)
        {
            int x = xi + 1;
            Complex p(dtau * potentialAt(x, y) / (4.0f * hbar), 0.0f);
            diagBuf_[xi] = 1.0f + 2.0f * alpha + p;
            Complex yNeighbors = alpha * (state[index(x, y + 1)] - 2.0f * state[index(x, y)] + state[index(x, y - 1)]);
            rhsBuf_[xi] = state[index(x, y)] + yNeighbors - p * state[index(x, y)];
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
            Complex p(dtau * potentialAt(x, y) / (4.0f * hbar), 0.0f);
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

    state.swap(nextPsi_);
}

float EigenstateFinder::estimateEnergy(const std::vector<Complex>& state) const
{
    std::vector<Complex> hState(state.size(), Complex(0.0f, 0.0f));

    for (int y = 1; y < resolution_ - 1; ++y)
    {
        for (int x = 1; x < resolution_ - 1; ++x)
        {
            Complex laplacian = state[index(x + 1, y)] + state[index(x - 1, y)]
                               + state[index(x, y + 1)] + state[index(x, y - 1)]
                               - 4.0f * state[index(x, y)];
            laplacian /= (dx_ * dx_);

            hState[index(x, y)] = -hbar * hbar / (2.0f * mass) * laplacian
                                 + potentialAt(x, y) * state[index(x, y)];
        }
    }

    return innerProduct(state, hState).real();
}

std::vector<EigenstateFinder::Complex> EigenstateFinder::seedWithAngularMomentum(int m)
{
    std::vector<Complex> candidate(resolution_ * resolution_);

    for (int y = 0; y < resolution_; ++y)
    {
        for (int x = 0; x < resolution_; ++x)
        {
            float worldX = (static_cast<float>(x) + 0.5f) * dx_ - size_ * 0.5f;
            float worldY = (static_cast<float>(y) + 0.5f) * dx_ - size_ * 0.5f;

            float theta = std::atan2(worldY, worldX);
            float noise = uniformDistribution_(generator_) - 0.5f;

            // random real envelope, times e^{i*m*theta} for the angular pattern
            Complex angular(std::cos(m * theta), std::sin(m * theta));
            candidate[index(x, y)] = noise * angular;
        }
    }

    for (int x = 0; x < resolution_; ++x)
    {
        candidate[index(x, 0)] = Complex(0.0f, 0.0f);
        candidate[index(x, resolution_ - 1)] = Complex(0.0f, 0.0f);
    }
    for (int y = 0; y < resolution_; ++y)
    {
        candidate[index(0, y)] = Complex(0.0f, 0.0f);
        candidate[index(resolution_ - 1, y)] = Complex(0.0f, 0.0f);
    }

    return candidate;
}

EigenstateFinder::Result EigenstateFinder::findNext(int m, int iterations, float dtau)
{
    std::vector<Complex> candidate = seedWithAngularMomentum(m);
    normalizeBuffer(candidate);

    int radialLevel = 0;
    for (const auto& [foundM, foundState] : foundStates_)
    {
        if (foundM == m)
        {
            ++radialLevel;
        }
    }

    for (int iter = 0; iter < iterations; ++iter)
    {
        stepImaginary(dtau, candidate);

        // only deflate against previously-found states with the SAME m --
        // different-m states are already orthogonal for free (see explanation),
        // so projecting against them would just be wasted work (and adds
        // needless numerical noise).
        for (const auto& [foundM, foundState] : foundStates_)
        {
            if (foundM == m)
            {
                projectOut(candidate, foundState);
            }
        }

        normalizeBuffer(candidate);
    }

    Result result;
    result.energy = estimateEnergy(candidate);
    result.psi = candidate;
    result.m = m;
    result.radialLevel = radialLevel;

    foundStates_.emplace_back(m, candidate);

    return result;
}