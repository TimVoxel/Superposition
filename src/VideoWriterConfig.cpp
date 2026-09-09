#include <VideoWriterConfig.hpp>

void from_json(const nlohmann::json& json, VideoWriterConfig& config)
{
    json.at("width").get_to(config.width);
    json.at("height").get_to(config.height);
    json.at("fps").get_to(config.fps);
    config.codec = json.value("codec", config.codec);
    config.pixelFormat = json.value("pixelFormat", config.pixelFormat);
    config.fileExtension = json.value("fileExtension", config.fileExtension);

    config.extraArgs.clear();
    for (const auto& arg : json.value("extraArgs", nlohmann::json::array()))
    {
        config.extraArgs.emplace_back(
            arg.at("flag").get<std::string>(),
            arg.at("value").get<std::string>()
        );
    }
}
