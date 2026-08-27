#pragma once

#include <SceneObject.hpp>
#include <nlohmann/json.hpp>
#include <vector>

class Scene
{
public:
    explicit Scene(const std::string& name, std::vector<std::unique_ptr<SceneObject>> particleSystems);

    static Scene fromJson(const nlohmann::json& json);

    void start(float currentTime);
    void update(float deltaTime, float currentTime);

    const std::vector<std::unique_ptr<SceneObject>>& objects() const;
    const std::string& name() const;
private:
    std::vector<std::unique_ptr<SceneObject>> objects_;
    std::string name_;
};