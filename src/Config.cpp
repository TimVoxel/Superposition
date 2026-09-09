#include <Config.hpp>
#include <nlohmann/json.hpp>
#include <Color.hpp>

void from_json(const nlohmann::json& json, Config& config)
{
    json.at("video").get_to(config.video);
    json["rendering"]["clearColor"].get_to(config.clearColor);
};
