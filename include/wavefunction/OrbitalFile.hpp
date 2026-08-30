#pragma once

#include <complex>
#include <string>
#include <vector>
#include <cstdint>

struct OrbitalData
{
    int resolution;
    float size;
    float a0;
    float energy;
    int n;
    int l;
    int m;
    int radialLevel;
    std::vector<std::complex<float>> psi;
};

namespace OrbitalFile
{
    void write(const std::string& path, const OrbitalData& data);
    OrbitalData read(const std::string& path);

    static const uint32_t kMagic = 0x48594F33;
}