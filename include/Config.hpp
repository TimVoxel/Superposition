#pragma once

#include <nlohmann/json.hpp>
#include <Color.hpp>
#include <VideoWriterConfig.hpp>

struct Config
{
    VideoWriterConfig video;
    Color clearColor;
};

void from_json(const nlohmann::json& json, Config& config);
