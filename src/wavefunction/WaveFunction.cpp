#include <stdexcept>
#include <wavefunction/GuassianWaveFunction.hpp>
#include <wavefunction/HydrogenWaveFunction.hpp>

std::unique_ptr<WaveFunction> WaveFunction::fromJson(const nlohmann::json& json)
{
    const auto type = json["type"].get<std::string>();

    if (type == "gaussian")
    {
        return std::make_unique<GaussianWaveFunction>(json["sigma"].get<float>());
    }
    if (type == "hydrogen")
    {
        return json.contains("orbital")
            ? std::make_unique<HydrogenWaveFunction>(json.at("orbital").get<std::string>(), json.value("timeStep", 0.0001f))
            : std::make_unique<HydrogenWaveFunction>(
                json["resolution"].get<int>(),
                json["size"].get<float>(),
                json["a0"].get<float>(),
                json.value("timeStep", 0.0001f)
        );
    }
    throw std::runtime_error("Unknown wave function type: " + type);
}