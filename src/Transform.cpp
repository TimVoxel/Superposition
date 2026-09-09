#include <Transform.hpp>

void from_json(const nlohmann::json& json, Transform& transform)
{
    json.at("pos").get_to(transform.position);
    json.at("scale").get_to(transform.scale);
    transform.opacity = json.value("opacity", 1.0f);
}
