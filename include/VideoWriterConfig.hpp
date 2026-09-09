#pragma once

#include <string>
#include <utility>
#include <vector>
#include <nlohmann/json.hpp>

struct VideoWriterConfig
{
    int width;
    int height;
    int fps;
    std::string codec = "libx264";
    std::string pixelFormat = "yuv420p";
    std::string fileExtension = "mp4";

    // Ordered codec-specific ffmpeg flags, e.g. {"crf", "0"} or {"profile:v", "4444"}.
    // Rendered as "-<flag> <value>" between "-c:v <codec>" and "-pix_fmt <pixelFormat>".
    std::vector<std::pair<std::string, std::string>> extraArgs;
};

void from_json(const nlohmann::json& json, VideoWriterConfig& config);
