#include <Color.hpp>

void from_json(const nlohmann::json& json, Color& color)
{
    color.r = json.at(0).get<float>();
    color.g = json.at(1).get<float>();
    color.b = json.at(2).get<float>();
    color.a = json.at(3).get<float>();
}