#include <wavefunction/OrbitalFile.hpp>

#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

constexpr double kPi = 3.14159265358979323846;

double factorial(int n)
{
    double result = 1.0;
    for (int i = 2; i <= n; ++i)
    {
        result *= i;
    }
    return result;
}

double associatedLegendre(int l, int m, double x)
{
    double pmm = 1.0;
    if (m > 0)
    {
        double somx2 = std::sqrt((1.0 - x) * (1.0 + x));
        double fact = 1.0;
        for (int i = 1; i <= m; ++i)
        {
            pmm *= -fact * somx2;
            fact += 2.0;
        }
    }

    if (l == m)
    {
        return pmm;
    }

    double pmmp1 = x * (2.0 * m + 1.0) * pmm;
    if (l == m + 1)
    {
        return pmmp1;
    }

    double pll = 0.0;
    for (int ll = m + 2; ll <= l; ++ll)
    {
        pll = (x * (2.0 * ll - 1.0) * pmmp1 - (ll + m - 1.0) * pmm) / (ll - m);
        pmm = pmmp1;
        pmmp1 = pll;
    }
    return pll;
}

double associatedLaguerre(int n, int k, double x)
{
    if (n == 0)
    {
        return 1.0;
    }

    double lPrev = 1.0;
    double lCurr = 1.0 + k - x;

    if (n == 1)
    {
        return lCurr;
    }

    for (int i = 2; i <= n; ++i)
    {
        double lNext = ((2.0 * i - 1.0 + k - x) * lCurr - (i - 1.0 + k) * lPrev) / i;
        lPrev = lCurr;
        lCurr = lNext;
    }
    return lCurr;
}

// Returns the signed real amplitude psi_nlm(r, theta), evaluated at phi=0.
double waveAmplitude(int n, int l, int m, float a0, double r, double cosTheta)
{
    if (r == 0.0 && l != 0)
    {
        return 0.0;
    }

    double rho = 2.0 * r / (n * a0);

    double radialNorm = std::sqrt(
        std::pow(2.0 / (n * a0), 3) *
        factorial(n - l - 1) /
        (2.0 * n * factorial(n + l))
    );

    double radial = std::exp(-rho / 2.0) * std::pow(rho, l)
                   * associatedLaguerre(n - l - 1, 2 * l + 1, rho);

    int absM = std::abs(m);
    double angularNorm = std::sqrt(
        (2.0 * l + 1.0) / (4.0 * kPi) *
        factorial(l - absM) / factorial(l + absM)
    );

    // phi = 0 slice: e^{i*m*phi} = 1, so amplitude is purely real here.
    double angular = angularNorm * associatedLegendre(l, absM, cosTheta);

    return radialNorm * radial * angular;
}

int main(int argc, char** argv)
{
    int n = 2;
    int l = 1;
    int m = 0;
    int resolution = 64;
    float a0 = 1.0f;
    float extent = 20.0f;
    std::string outputPath = "orbital.bin";

    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];
        if (arg == "-n" && i + 1 < argc) n = std::stoi(argv[++i]);
        else if (arg == "-l" && i + 1 < argc) l = std::stoi(argv[++i]);
        else if (arg == "-m" && i + 1 < argc) m = std::stoi(argv[++i]);
        else if (arg == "-r" && i + 1 < argc) resolution = std::stoi(argv[++i]);
        else if (arg == "-a0" && i + 1 < argc) a0 = std::stof(argv[++i]);
        // else if (arg == "-extent" && i + 1 < argc) extent = std::stof(argv[++i]);
        else if (arg == "-o" && i + 1 < argc) outputPath = argv[++i];
        else
        {
            std::cerr << "Unrecognized argument: " << arg << "\n";
            return 1;
        }
    }

    if (l < 0 || l > n - 1)
    {
        std::cerr << "Invalid l for n=" << n << " (must be 0.." << (n - 1) << ")\n";
        return 1;
    }
    if (std::abs(m) > l)
    {
        std::cerr << "Invalid m for l=" << l << " (must be -" << l << ".." << l << ")\n";
        return 1;
    }

    // Auto-search for an extent where the tail has genuinely decayed,
    // instead of trusting a fixed guess.
    double searchExtent = 5.0 * n * n; // generous starting guess
    double peakAmp = 0.0;

    for (int attempt = 0; attempt < 20; ++attempt)
    {
        peakAmp = 0.0;
        double sampleStep = searchExtent * a0 / 200.0;
        for (double r = 0.0; r <= searchExtent * a0; r += sampleStep)
        {
            double amp = std::abs(waveAmplitude(n, l, m, a0, r, 1.0)); // theta=0 axis, generous
            peakAmp = std::max(peakAmp, amp);
        }

        double edgeAmp = std::abs(waveAmplitude(n, l, m, a0, searchExtent * a0, 1.0));

        if (peakAmp > 0.0 && edgeAmp / peakAmp < 0.001)
        {
            break; // tail has decayed enough
        }

        searchExtent *= 1.5; // not decayed yet -- widen and retry
    }

    std::cout << "Auto-selected extent: " << searchExtent << "\n";
    extent = static_cast<float>(searchExtent);

    std::cout << "Evaluating psi_" << n << l << m << " on a "
               << resolution << "x" << resolution << " slice...\n";

    float size = 2.0f * extent * a0;
    float dx = size / resolution;

    OrbitalData data;
    data.resolution = resolution;
    data.size = size;
    data.a0 = a0;
    data.n = n;
    data.l = l;
    data.m = m;
    data.radialLevel = n - l - 1; // number of radial nodes, matches 2D solver's convention
    data.energy = -1.0f / (2.0f * n * n); // Bohr formula, atomic units (hbar=m=k=1, matching a0=1 convention)
    data.psi.resize(static_cast<std::size_t>(resolution) * resolution);

    for (int row = 0; row < resolution; ++row)
    {
        for (int col = 0; col < resolution; ++col)
        {
            // same worldX/worldY convention as HydrogenWaveFunction, so
            // sample()/render code needs no changes to consume this file.
            double xh = (col + 0.5) * dx - size * 0.5;
            double z  = (row + 0.5) * dx - size * 0.5;

            double r = std::sqrt(xh * xh + z * z);
            double cosTheta = (r > 0.0) ? (z / r) : 1.0;

            double amplitude = waveAmplitude(n, l, m, a0, r, cosTheta);
            data.psi[row * resolution + col] = std::complex<float>(
                static_cast<float>(amplitude), 0.0f
            );
        }
    }

    double maxAmplitude = 0.0;
    double maxR = 0.0;
    for (int row = 0; row < resolution; ++row)
    {
        for (int col = 0; col < resolution; ++col)
        {
            double xh = (col + 0.5) * dx - size * 0.5;
            double z  = (row + 0.5) * dx - size * 0.5;
            double amp = std::abs(waveAmplitude(n, l, m, a0, std::sqrt(xh*xh+z*z),
                                                (std::sqrt(xh*xh+z*z) > 0.0 ? z/std::sqrt(xh*xh+z*z) : 1.0)));
            if (amp > maxAmplitude)
            {
                maxAmplitude = amp;
                maxR = std::sqrt(xh * xh + z * z);
            }
        }
    }
    std::cout << "Peak amplitude at r = " << maxR
            << " (extent was " << extent * a0 << ")\n";

    OrbitalFile::write(outputPath, data);
    std::cout << "Wrote " << outputPath << " (energy = " << data.energy << ")\n";

    return 0;
}