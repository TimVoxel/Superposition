#include "VideoWriter.hpp"

#include <iostream>

bool VideoWriter::open(int width, int height, int fps, const std::string& filename)
{
    std::string command =
        "ffmpeg "
        "-y "
        "-f rawvideo "
        "-pixel_format rgba "
        "-video_size " + std::to_string(width) + "x" + std::to_string(height) + " "
        "-framerate " + std::to_string(fps) + " "
        "-i pipe:0 "
        "-c:v libx264 "
        "-pix_fmt yuv420p "
        "\"" + filename + "\"";

    pipe_ = _popen(command.c_str(), "wb");

    if (pipe_ == nullptr)
    {
        std::cerr << "Failed to start FFmpeg\n";
        return false;
    }
    return true;
}

bool VideoWriter::writeFrame(const unsigned char* pixels, size_t size)
{
    if (pipe_ == nullptr)
    {
        return false;
    }

    return fwrite(pixels, 1, size, pipe_) == size;
}

void VideoWriter::close()
{
    if (pipe_ == nullptr)
    {
        return;
    }

    _pclose(pipe_);
    pipe_ = nullptr;
}

VideoWriter::~VideoWriter()
{
    close();
}