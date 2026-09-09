#include <ParticleSystemConfig.hpp>
#include <Color.hpp>
#include <Math.hpp>

void from_json(const nlohmann::json& json, ParticleSystemConfig& config)
{
    json.at("growSeconds").get_to(config.growSeconds);
    json.at("sustainSeconds").get_to(config.sustainSeconds);
    json.at("maxSpawnRatePS").get_to(config.maxSpawnRatePS);
    json.at("maxParticles").get_to(config.maxParticles);
    json.at("lifetime").get_to(config.lifetime);
    json.at("pointSize").get_to(config.pointSize);
    json.at("sizeIncrease").get_to(config.sizeIncrease);
    json.at("amplitude").get_to(config.amplitude);
    json.at("frequency").get_to(config.frequency);
    json.at("color").get_to(config.color);
};
