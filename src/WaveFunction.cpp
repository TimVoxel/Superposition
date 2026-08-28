#include <stdexcept>
#include <wavefunction/GuassianWaveFunction.hpp>

std::unique_ptr<WaveFunction> WaveFunction::fromJson(const nlohmann::json& json)
{
    const auto type = json["type"].get<std::string>();

    if (type == "gaussian")
        return std::make_unique<GaussianWaveFunction>(
            json["sigma"].get<float>()
        );

    throw std::runtime_error("Unknown wave function type: " + type);
}