// Config.cpp: Game configuration manager
//

#include "Config.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <sys/stat.h>

// Global config instance
GameConfig g_config;

GameConfig::GameConfig()
    : speed(1.5f)
    , deadmatchBombkill(true)
    , explodePredictVisible(true)
    , langFile(1)  // Czech
    , soundAvailable(true)
    , fullscreen(false)
    , maxMrchovnik(0)
{
}

std::string GameConfig::GetDefaultPath()
{
    std::string path;

    // Try XDG_CONFIG_HOME first
    const char* xdgConfig = std::getenv("XDG_CONFIG_HOME");
    if (xdgConfig != nullptr && xdgConfig[0] != '\0') {
        path = std::string(xdgConfig) + "/bombic";
    } else {
        // Fall back to ~/.config/bombic
        const char* home = std::getenv("HOME");
        if (home != nullptr) {
            path = std::string(home) + "/.config/bombic";
        } else {
            // Last resort: current directory
            path = ".";
        }
    }

    // Create directory if it doesn't exist
    mkdir(path.c_str(), 0755);

    return path + "/bombic.conf";
}

std::string GameConfig::Trim(const std::string& str)
{
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";

    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

void GameConfig::ParseLine(const std::string& line)
{
    // Skip empty lines and comments
    std::string trimmed = Trim(line);
    if (trimmed.empty() || trimmed[0] == '#' || trimmed[0] == ';') {
        return;
    }

    // Find the '=' separator
    size_t eqPos = trimmed.find('=');
    if (eqPos == std::string::npos) {
        return;
    }

    std::string key = Trim(trimmed.substr(0, eqPos));
    std::string value = Trim(trimmed.substr(eqPos + 1));

    // Parse known keys
    if (key == "speed") {
        speed = std::stof(value);
        if (speed < 1.0f) speed = 1.0f;
        if (speed > 4.0f) speed = 4.0f;
    }
    else if (key == "deadmatch_bombkill") {
        deadmatchBombkill = (value == "true" || value == "1");
    }
    else if (key == "explode_predict_visible") {
        explodePredictVisible = (value == "true" || value == "1");
    }
    else if (key == "language") {
        langFile = std::stoi(value);
    }
    else if (key == "sound") {
        soundAvailable = (value == "true" || value == "1");
    }
    else if (key == "fullscreen") {
        fullscreen = (value == "true" || value == "1");
    }
    else if (key == "max_mrchovnik") {
        maxMrchovnik = std::stoi(value);
    }
}

bool GameConfig::Load(const std::string& filename)
{
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Config: Could not open " << filename << ", using defaults" << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        try {
            ParseLine(line);
        } catch (const std::exception& e) {
            std::cerr << "Config: Error parsing line: " << line << " - " << e.what() << std::endl;
        }
    }

    std::cout << "Config: Loaded from " << filename << std::endl;
    return true;
}

bool GameConfig::Save(const std::string& filename) const
{
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Config: Could not save to " << filename << std::endl;
        return false;
    }

    file << "# Bombic configuration file\n";
    file << "# Generated automatically\n\n";

    file << "[game]\n";
    file << "speed = " << speed << "\n";
    file << "deadmatch_bombkill = " << (deadmatchBombkill ? "true" : "false") << "\n";
    file << "explode_predict_visible = " << (explodePredictVisible ? "true" : "false") << "\n";
    file << "max_mrchovnik = " << maxMrchovnik << "\n";
    file << "\n";

    file << "[display]\n";
    file << "fullscreen = " << (fullscreen ? "true" : "false") << "\n";
    file << "\n";

    file << "[audio]\n";
    file << "sound = " << (soundAvailable ? "true" : "false") << "\n";
    file << "\n";

    file << "[locale]\n";
    file << "# 0 = English, 1 = Czech\n";
    file << "language = " << langFile << "\n";

    std::cout << "Config: Saved to " << filename << std::endl;
    return true;
}
