#include <Math.hpp>

void from_json(const nlohmann::json& json, Vec2f& v)
{
    json.at(0).get_to(v.x);
    json.at(1).get_to(v.y);
}

void from_json(const nlohmann::json& json, Vec3f& v)
{
    json.at(0).get_to(v.x);
    json.at(1).get_to(v.y);
    json.at(2).get_to(v.z);
}

void from_json(const nlohmann::json& json, Vec4f& v)
{
    json.at(0).get_to(v.x);
    json.at(1).get_to(v.y);
    json.at(2).get_to(v.z);
    json.at(3).get_to(v.w);
}