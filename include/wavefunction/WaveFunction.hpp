#pragma once

#include <utility>
#include <memory>
#include <nlohmann/json.hpp>

class WaveFunction
{
public:
    virtual ~WaveFunction() = default;
    virtual std::pair<float, float> sample(float time) = 0;
    static std::unique_ptr<WaveFunction> fromJson(const nlohmann::json& json);
};
