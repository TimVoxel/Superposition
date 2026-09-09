#pragma once

#include <string>

enum class RunMode
{
    Display,
    Render
};

struct CommandLineArgs
{
    RunMode mode = RunMode::Display;
    std::string scenePath = "resources/scene1.json";
    std::string outputDir = "output/";
};

CommandLineArgs parseCommandLineArgs(int argc, char** argv);