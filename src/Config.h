#ifndef CONFIG_GAME_H_
#define CONFIG_GAME_H_

#include <string>

// Game configuration manager
// Handles loading/saving settings to a human-readable INI file

class GameConfig {
public:
    // Game settings
    float speed;
    bool deadmatchBombkill;
    bool explodePredictVisible;
    int langFile;
    bool soundAvailable;
    bool fullscreen;
    int maxMrchovnik;

    GameConfig();

    // Load configuration from file, returns true on success
    bool Load(const std::string& filename);

    // Save configuration to file, returns true on success
    bool Save(const std::string& filename) const;

    // Get default config path in user's home directory
    static std::string GetDefaultPath();

private:
    // Parse a single line from config file
    void ParseLine(const std::string& line);

    // Trim whitespace from string
    static std::string Trim(const std::string& str);
};

// Global config instance
extern GameConfig g_config;

#endif // CONFIG_GAME_H_
