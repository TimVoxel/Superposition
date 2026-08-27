#pragma once

#include <nlohmann/json.hpp>

struct Vec2f
{
    float x;
    float y;
};

struct Vec3f
{
    float x;
    float y;
    float z;
};

struct Vec4f
{
    float x;
    float y;
    float z;
    float w;
};

void from_json(const nlohmann::json& json, Vec2f& v);
void from_json(const nlohmann::json& json, Vec3f& v);
void from_json(const nlohmann::json& json, Vec4f& v);