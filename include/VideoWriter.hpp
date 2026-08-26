#pragma once

#include <cstdio>
#include <string>

class VideoWriter
{
public:
    bool open(int width, int height, int fps, const std::string& filename);
    bool writeFrame(const unsigned char* pixels, size_t size);
    void close();
    ~VideoWriter();

private:
    FILE* pipe_ = nullptr;
};