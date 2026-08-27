#include <Config.hpp>
#include <nlohmann/json.hpp>
#include <Color.hpp>

void from_json(const nlohmann::json& json, Config& config)
{
    json["video"]["width"].get_to(config.width);
    json["video"]["height"].get_to(config.height);
    json["video"]["fps"].get_to(config.fps);
    json["video"]["durationSeconds"].get_to(config.durationSeconds);
    json["rendering"]["clearColor"].get_to(config.clearColor);
};