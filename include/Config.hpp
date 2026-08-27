#pragma once

#include <nlohmann/json.hpp>
#include <Color.hpp>

struct Config
{
    int width;
    int height;
    int fps;
    int durationSeconds;
    Color clearColor;
};

void from_json(const nlohmann::json& json, Config& config);