#include <CommandLineArgs.hpp>
#include <iostream>

CommandLineArgs parseCommandLineArgs(int argc, char** argv)
{
    CommandLineArgs args;

    for (int i = 1; i < argc; ++i)
    {
        std::string argument = argv[i];

        if (argument == "-r")
        {
            args.mode = RunMode::Render;
        }
        else if (argument == "-d")
        {
            args.mode = RunMode::Display;
        }
        else if (argument == "-s")
        {
            if (i + 1 >= argc)
            {
                std::cerr << "Missing scene path after -s\n";
                continue;
            }
            args.scenePath = argv[++i];
        }
        else if (argument == "-o")
        {
            if (i + 1 >= argc)
            {
                std::cerr << "Missing output directory after -o\n";
                continue;
            }
            args.outputDir = argv[++i];
        }
        else
        {
            std::cout << "Unknown option \"" << argument << "\": -r, -d, -s <scene>, -o <outputDir\n";
        }
    }
    return args;
}