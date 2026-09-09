#pragma once

#include <Math.hpp>
#include <nlohmann/json.hpp>

struct Transform
{
    Vec2f position;
    Vec2f scale;
    float opacity = 1.0f;
};

void from_json(const nlohmann::json& json, Transform& transform);
