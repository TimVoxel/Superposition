#include <Config.hpp>
#include <nlohmann/json.hpp>

Color colorFromJson(const nlohmann::json& json)
{
    return {
        json[0].get<float>(),
        json[1].get<float>(),
        json[2].get<float>(),
        json[3].get<float>()
    };
}

Config configFromJson(const nlohmann::json& json)
{
    return {
        .width = json["screen"]["width"].get<int>(),
        .height = json["screen"]["height"].get<int>(),
        .fps = json["screen"]["fps"].get<int>(),

        .pointSize = json["particles"]["pointSize"].get<float>(),
        .sizeIncrease = json["particles"]["sizeIncrease"].get<float>(),
        .durationSeconds = json["simulation"]["durationSeconds"].get<float>(),
        .fadeoutSeconds = json["simulation"]["fadeoutSeconds"].get<float>(),
        .maxSpawnRatePS = json["particles"]["maxSpawnRatePS"].get<float>(),

        .clearColor = colorFromJson(json["colors"]["clear"]),
        .pointColor = colorFromJson(json["colors"]["point"])
    };
}