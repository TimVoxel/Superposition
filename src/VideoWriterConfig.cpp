#include <VideoWriterConfig.hpp>

void from_json(const nlohmann::json& json, VideoWriterConfig& config)
{
    json.at("width").get_to(config.width);
    json.at("height").get_to(config.height);
    json.at("fps").get_to(config.fps);
    config.codec = json.value("codec", config.codec);
    config.crf = json.value("crf", config.crf);
    config.preset = json.value("preset", config.preset);
    config.pixelFormat = json.value("pixelFormat", config.pixelFormat);
}
