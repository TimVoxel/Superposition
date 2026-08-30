#include "VideoWriter.hpp"

#include <iostream>

bool VideoWriter::open(const VideoWriterConfig& config, const std::string& filename)
{
    std::string command =
        "ffmpeg "
        "-y "
        "-f rawvideo "
        "-pixel_format rgba "
        "-video_size " + std::to_string(config.width) + "x" + std::to_string(config.height) + " "
        "-framerate " + std::to_string(config.fps) + " "
        "-i pipe:0 "
        "-c:v " + config.codec + " "
        "-crf " + std::to_string(config.crf) + " "
        "-preset " + config.preset + " "
        "-pix_fmt " + config.pixelFormat + " "
        "\"" + filename + "\"";

    pipe_ = _popen(command.c_str(), "wb");

    if (pipe_ == nullptr)
    {
        std::cerr << "Failed to start FFmpeg\n";
        return false;
    }
    return true;
}

bool VideoWriter::writeFrame(const unsigned char* pixels, int width, int height)
{
    if (pipe_ == nullptr)
    {
        return false;
    }

    const size_t rowSize = static_cast<size_t>(width) * 4;
    const size_t frameSize = rowSize * static_cast<size_t>(height);

    if (frameSize != buffer_.size())
    {
        buffer_.resize(frameSize);
    }

    for (int y = 0; y < height; ++y)
    {
        std::memcpy(
            buffer_.data() + static_cast<size_t>(y) * rowSize,
            pixels + static_cast<size_t>(height - 1 - y) * rowSize,
            rowSize
        );
    }
    return fwrite(buffer_.data(), 1, frameSize, pipe_) == frameSize;
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
