#pragma once

#include <cstdio>
#include <string>
#include <vector>
#include <cstring>
#include <VideoWriterConfig.hpp>

class VideoWriter
{
public:
    bool open(const VideoWriterConfig& config, const std::string& filename);
    bool writeFrame(const unsigned char* pixels, int width, int height);
    void close();
    ~VideoWriter();

private:
    FILE* pipe_ = nullptr;
    std::vector<unsigned char> buffer_;
};
