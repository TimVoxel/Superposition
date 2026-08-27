#pragma once
#include <nlohmann/json.hpp>

struct Color
{
    float r;
    float g;
    float b;
    float a;
};

void from_json(const nlohmann::json& json, Color& color);
