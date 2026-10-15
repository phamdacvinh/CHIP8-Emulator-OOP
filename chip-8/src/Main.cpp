#include "Chip8.hpp"
#include "Platform.hpp"

#include <algorithm>
#include <chrono>
#include <cctype>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace
{
namespace fs = std::filesystem;

bool CanOpen(std::string const& filename)
{
    std::ifstream file(filename, std::ios::binary);
    return file.good();
}

std::string ToLower(std::string value)
{
    for (char& ch : value)
    {
        ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
    }

    return value;
}

std::string NormalizeName(std::string value)
{
    std::string normalized;

    for (char ch : value)
    {
        unsigned char uch = static_cast<unsigned char>(ch);
        if (std::isalnum(uch))
        {
            normalized += static_cast<char>(std::tolower(uch));
        }
    }

    return normalized;
}

std::vector<fs::path> ROMDirectories()
{
    return {
        fs::path("roms"),
        fs::path("../roms"),
    };
}

std::vector<fs::path> FindROMMatches(std::string const& requestedROM)
{
    std::vector<fs::path> matches;
    std::string requestedName = NormalizeName(fs::path(requestedROM).stem().string());

    if (requestedName.empty())
    {
        return matches;
    }

    for (fs::path const& directory : ROMDirectories())
    {
        if (!fs::exists(directory))
        {
            continue;
        }

        for (fs::directory_entry const& entry : fs::directory_iterator(directory))
        {
            if (!entry.is_regular_file() || ToLower(entry.path().extension().string()) != ".ch8")
            {
                continue;
            }

            std::string romName = NormalizeName(entry.path().stem().string());
            if (romName == requestedName)
            {
                return {entry.path()};
            }

            if (romName.find(requestedName) != std::string::npos)
            {
                matches.push_back(entry.path());
            }
        }
    }

    std::sort(matches.begin(), matches.end());
    return matches;
}

std::vector<fs::path> ListROMs()
{
    std::vector<fs::path> roms;

    for (fs::path const& directory : ROMDirectories())
    {
        if (!fs::exists(directory))
        {
            continue;
        }

        for (fs::directory_entry const& entry : fs::directory_iterator(directory))
        {
            if (entry.is_regular_file() && ToLower(entry.path().extension().string()) == ".ch8")
            {
                roms.push_back(entry.path());
            }
        }
    }

    std::sort(roms.begin(), roms.end());
    return roms;
}

std::string ResolveROM(std::string const& requestedROM)
{
    std::string rom = requestedROM;
    std::string alias = ToLower(requestedROM);

    if (alias == "ibm")
    {
        rom = "IBM.ch8";
    }
    else if (alias == "breakout")
    {
        rom = "Breakout.ch8";
    }
    else if (alias == "dodge")
    {
        rom = "Dodge.ch8";
    }

    std::string candidates[] = {
        requestedROM,
        "roms/" + rom,
        "../roms/" + rom,
        "roms/" + rom + ".ch8",
        "../roms/" + rom + ".ch8",
    };

    for (std::string const& candidate : candidates)
    {
        if (CanOpen(candidate))
        {
            return candidate;
        }
    }

    std::vector<fs::path> matches = FindROMMatches(requestedROM);
    if (!matches.empty())
    {
        if (matches.size() > 1)
        {
            std::cerr << "Multiple games match \"" << requestedROM << "\". Using "
                      << matches.front().filename().string() << ". Other matches:\n";

            for (size_t i = 1; i < matches.size() && i < 8; ++i)
            {
                std::cerr << "  " << matches[i].filename().string() << '\n';
            }
        }

        return matches.front().string();
    }

    return requestedROM;
}

void PrintUsage(char const* executable)
{
    std::cerr
        << "Usage:\n"
        << "  " << executable << " <Game>\n"
        << "  " << executable << " <Scale> <Delay> <ROM or Game>\n\n"
        << "Game can be a ROM path, exact file name, or part of a game name.\n"
        << "Examples:\n"
        << "  " << executable << " tetris\n"
        << "  " << executable << " \"space invaders\"\n"
        << "  " << executable << " 10 8 dodge\n"
        << "  " << executable << " --list\n";
}
}

int main(int argc, char** argv)
{
    int videoScale = 10;
    int cycleDelay = 8;
    std::string romFilename;

    if (argc == 2 && std::string(argv[1]) == "--list")
    {
        for (fs::path const& rom : ListROMs())
        {
            std::cout << rom.filename().string() << '\n';
        }

        return 0;
    }

    if (argc == 2)
    {
        romFilename = ResolveROM(argv[1]);
    }
    else if (argc == 4)
    {
        try
        {
            videoScale = std::stoi(argv[1]);
            cycleDelay = std::stoi(argv[2]);
        }
        catch (std::exception const& error)
        {
            std::cerr << "Invalid scale or delay: " << error.what() << '\n';
            PrintUsage(argv[0]);
            std::exit(EXIT_FAILURE);
        }

        romFilename = ResolveROM(argv[3]);
    }
    else
    {
        PrintUsage(argv[0]);
        std::exit(EXIT_FAILURE);
    }

    Chip8 chip8;
    if (!chip8.LoadROM(romFilename.c_str()))
    {
        PrintUsage(argv[0]);
        std::exit(EXIT_FAILURE);
    }

    Platform platform(
        ("CHIP-8 Emulator - " + romFilename).c_str(),
        VIDEO_WIDTH * videoScale,
        VIDEO_HEIGHT * videoScale,
        VIDEO_WIDTH,
        VIDEO_HEIGHT
    );

    int videoPitch = sizeof(chip8.video[0]) * VIDEO_WIDTH;

    auto lastCycleTime = std::chrono::high_resolution_clock::now();
    bool quit = false;

    while (!quit)
    {
        quit = platform.ProcessInput(chip8.keypad);

        auto currentTime = std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(
            currentTime - lastCycleTime
        ).count();

        if (dt > cycleDelay)
        {
            lastCycleTime = currentTime;
            chip8.Cycle();
            platform.Update(chip8.video, videoPitch);
        }
    }

    return 0;
}
