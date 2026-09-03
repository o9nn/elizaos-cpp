#pragma once

#include "elizaos/characters.hpp"
#include <string>
#include <vector>
#include <optional>

namespace elizaos {

/**
 * @brief Utility class for loading characters from JSON files
 * 
 * This class provides functionality to load character definitions from
 * the standard ElizaOS character JSON format used in the /characters directory.
 */
class CharacterJsonLoader {
    // Grants the unit tests access to the private JSON parsing helpers without
    // resorting to `#define private public`, which is undefined behaviour and
    // breaks on MSVC because the access specifier is part of the decorated
    // (mangled) symbol name, producing LNK2019 at link time.
    friend struct CharacterJsonLoaderTestAccess;

public:
    /**
     * @brief Load a character from a JSON file
     * @param filepath Path to the JSON character file
     * @return CharacterProfile if successful, nullopt if failed
     */
    static std::optional<CharacterProfile> loadFromFile(const std::string& filepath);
    
    /**
     * @brief Load a character from JSON string
     * @param jsonString JSON content as string
     * @return CharacterProfile if successful, nullopt if failed
     */
    static std::optional<CharacterProfile> loadFromJsonString(const std::string& jsonString);
    
    /**
     * @brief Load all characters from a directory
     * @param directoryPath Path to directory containing character JSON files
     * @return Vector of loaded character profiles
     */
    static std::vector<CharacterProfile> loadFromDirectory(const std::string& directoryPath);
    
    /**
     * @brief Save a character to JSON file
     * @param character Character profile to save
     * @param filepath Path where to save the JSON file
     * @return true if successful, false otherwise
     */
    static bool saveToFile(const CharacterProfile& character, const std::string& filepath);
    
    /**
     * @brief Convert character to ElizaOS JSON format string
     * @param character Character profile to convert
     * @return JSON string representation
     */
    static std::string toJsonString(const CharacterProfile& character);

private:
    /**
     * @brief Parse personality matrix from JSON
     */
    static PersonalityMatrix parsePersonalityFromJson(const JsonValue& json);
    
    /**
     * @brief Parse communication style from JSON
     */
    static CommunicationStyle parseCommunicationStyleFromJson(const JsonValue& json);
    
    /**
     * @brief Parse character background from JSON
     */
    static CharacterBackground parseBackgroundFromJson(const JsonValue& json);
    
    /**
     * @brief Parse traits from JSON
     */
    static std::vector<CharacterTrait> parseTraitsFromJson(const JsonValue& json);
    
    /**
     * @brief Helper function to get string value from JSON
     */
    static std::string getStringFromJson(const JsonValue& json, const std::string& key, const std::string& defaultValue = "");
    
    /**
     * @brief Helper function to get string array from JSON
     */
    static std::vector<std::string> getStringArrayFromJson(const JsonValue& json, const std::string& key);
    
    /**
     * @brief Helper function to get float value from JSON
     */
    static float getFloatFromJson(const JsonValue& json, const std::string& key, float defaultValue = 0.5f);
};

/**
 * @brief Test-only accessor exposing CharacterJsonLoader's private helpers.
 *
 * Declared as a friend of CharacterJsonLoader so unit tests can exercise the
 * parsing helpers directly. Header-only forwarding keeps the library ABI and
 * the helpers' access level unchanged.
 */
struct CharacterJsonLoaderTestAccess {
    static PersonalityMatrix parsePersonality(const JsonValue& json) {
        return CharacterJsonLoader::parsePersonalityFromJson(json);
    }
    static CommunicationStyle parseCommunicationStyle(const JsonValue& json) {
        return CharacterJsonLoader::parseCommunicationStyleFromJson(json);
    }
    static CharacterBackground parseBackground(const JsonValue& json) {
        return CharacterJsonLoader::parseBackgroundFromJson(json);
    }
    static std::vector<CharacterTrait> parseTraits(const JsonValue& json) {
        return CharacterJsonLoader::parseTraitsFromJson(json);
    }
    static std::string getString(const JsonValue& json, const std::string& key,
                                 const std::string& defaultValue = "") {
        return CharacterJsonLoader::getStringFromJson(json, key, defaultValue);
    }
    static std::vector<std::string> getStringArray(const JsonValue& json,
                                                   const std::string& key) {
        return CharacterJsonLoader::getStringArrayFromJson(json, key);
    }
    static float getFloat(const JsonValue& json, const std::string& key,
                          float defaultValue = 0.5f) {
        return CharacterJsonLoader::getFloatFromJson(json, key, defaultValue);
    }
};

} // namespace elizaos