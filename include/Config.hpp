#pragma once

#include <nlohmann/json.hpp>
#include <Color.hpp>

struct Config
{
    int width;
    int height;

    float pointSize;
    float sizeIncrease;
    float durationSeconds;
    float maxSpawnRatePS;

    Color clearColor;
    Color pointColor;
};

Config configFromJson(const nlohmann::json& json);