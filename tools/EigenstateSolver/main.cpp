#include "EigenstateFinder.hpp"
#include <wavefunction/OrbitalFile.hpp>

#include <iostream>
#include <string>
#include <vector>

int main(int argc, char** argv)
{
    int resolution = 64;
    float size = 20.0f;
    float a0 = 1.0f;
    int maxM = 2;
    int radialLevelsPerM = 2;
    int iterations = 2000;
    float dtau = 0.001f;
    std::string outputPrefix = "hydrogen";

    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];
        if (arg == "-r" && i + 1 < argc) resolution = std::stoi(argv[++i]);
        else if (arg == "-s" && i + 1 < argc) size = std::stof(argv[++i]);
        else if (arg == "-a0" && i + 1 < argc) a0 = std::stof(argv[++i]);
        else if (arg == "-m" && i + 1 < argc) maxM = std::stoi(argv[++i]);
        else if (arg == "-radial" && i + 1 < argc) radialLevelsPerM = std::stoi(argv[++i]);
        else if (arg == "-iter" && i + 1 < argc) iterations = std::stoi(argv[++i]);
        else if (arg == "-dtau" && i + 1 < argc) dtau = std::stof(argv[++i]);
        else if (arg == "-o" && i + 1 < argc) outputPrefix = argv[++i];
    }

    std::cout << "Solving m = -" << maxM << ".." << maxM
               << ", " << radialLevelsPerM << " radial levels each, on a "
               << resolution << "x" << resolution << " grid...\n";

    EigenstateFinder finder(resolution, size, a0);

    for (int m = -maxM; m <= maxM; ++m)
    {
        for (int level = 0; level < radialLevelsPerM; ++level)
        {
            std::cout << "Finding m=" << m << " radial level " << level << "... " << std::flush;

            auto result = finder.findNext(m, iterations, dtau);

            std::cout << "energy = " << result.energy << "\n";

            OrbitalData data;
            data.resolution = resolution;
            data.size = size;
            data.a0 = a0;
            data.energy = result.energy;
            data.m = result.m;
            data.radialLevel = result.radialLevel;
            data.psi = std::move(result.psi);

            std::string path = outputPrefix
                + "_m" + std::to_string(m)
                + "_r" + std::to_string(level) + ".bin";
            OrbitalFile::write(path, data);

            std::cout << "  wrote " << path << "\n";
        }
    }

    std::cout << "Done.\n";
    return 0;
}