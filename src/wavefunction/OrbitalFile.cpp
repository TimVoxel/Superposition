#include <wavefunction/OrbitalFile.hpp>

#include <fstream>
#include <stdexcept>

void OrbitalFile::write(const std::string& path, const OrbitalData& data)
{
    std::ofstream file(path, std::ios::binary);
    if (!file)
    {
        throw std::runtime_error("Could not open orbital file for writing: " + path);
    }

    uint32_t magic = OrbitalFile::kMagic;
    file.write(reinterpret_cast<const char*>(&magic), sizeof(magic));
    file.write(reinterpret_cast<const char*>(&data.resolution), sizeof(data.resolution));
    file.write(reinterpret_cast<const char*>(&data.size), sizeof(data.size));
    file.write(reinterpret_cast<const char*>(&data.a0), sizeof(data.a0));
    file.write(reinterpret_cast<const char*>(&data.energy), sizeof(data.energy));
    file.write(reinterpret_cast<const char*>(&data.n), sizeof(data.n));
    file.write(reinterpret_cast<const char*>(&data.l), sizeof(data.l));
    file.write(reinterpret_cast<const char*>(&data.m), sizeof(data.m));
    file.write(reinterpret_cast<const char*>(&data.radialLevel), sizeof(data.radialLevel));

    const auto expectedCount =
        static_cast<std::size_t>(data.resolution) * static_cast<std::size_t>(data.resolution);
    if (data.psi.size() != expectedCount)
    {
        throw std::runtime_error("Orbital data size does not match resolution*resolution");
    }

    file.write(
        reinterpret_cast<const char*>(data.psi.data()),
        static_cast<std::streamsize>(data.psi.size() * sizeof(std::complex<float>))
    );

    if (!file)
    {
        throw std::runtime_error("Failed while writing orbital file: " + path);
    }
}

OrbitalData OrbitalFile::read(const std::string& path)
{
    std::ifstream file(path, std::ios::binary);
    if (!file)
    {
        throw std::runtime_error("Could not open orbital file for reading: " + path);
    }

    uint32_t magic = 0;
    file.read(reinterpret_cast<char*>(&magic), sizeof(magic));
    if (magic != OrbitalFile::kMagic)
    {
        throw std::runtime_error("Not a valid orbital file (bad magic, or old format): " + path);
    }

    OrbitalData data;
    file.read(reinterpret_cast<char*>(&data.resolution), sizeof(data.resolution));
    file.read(reinterpret_cast<char*>(&data.size), sizeof(data.size));
    file.read(reinterpret_cast<char*>(&data.a0), sizeof(data.a0));
    file.read(reinterpret_cast<char*>(&data.energy), sizeof(data.energy));
    file.read(reinterpret_cast<char*>(&data.n), sizeof(data.n));
    file.read(reinterpret_cast<char*>(&data.l), sizeof(data.l));
    file.read(reinterpret_cast<char*>(&data.m), sizeof(data.m));
    file.read(reinterpret_cast<char*>(&data.radialLevel), sizeof(data.radialLevel));

    if (data.resolution < 3)
    {
        throw std::runtime_error("Orbital file has invalid resolution: " + path);
    }

    const auto count =
        static_cast<std::size_t>(data.resolution) * static_cast<std::size_t>(data.resolution);
    data.psi.resize(count);

    file.read(
        reinterpret_cast<char*>(data.psi.data()),
        static_cast<std::streamsize>(count * sizeof(std::complex<float>))
    );

    if (!file)
    {
        throw std::runtime_error("Orbital file is truncated or corrupt: " + path);
    }

    return data;
}