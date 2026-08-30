#pragma once

#include <string>
#include <nlohmann/json.hpp>

struct VideoWriterConfig
{
    int width;
    int height;
    int fps;
    std::string codec = "libx264";
    int crf = 0;
    std::string preset = "veryslow";
    std::string pixelFormat = "yuv420p";
};

void from_json(const nlohmann::json& json, VideoWriterConfig& config);
