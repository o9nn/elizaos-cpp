#include "loader.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::optional<std::string> tryLoadFile(const std::string& filePath) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Check for --characters flag
    const auto args = process.argv;
    const auto charactersFlag = args.find((arg) => arg.startsWith("--characters="));
    const auto characterPath = charactersFlag.split("=")[1];

    if (characterPath) {
        filePath = characterPath;
    }

    try {
        return fs.readFileSync(filePath, "utf8");
        } catch (e) {
            return nullptr;
        }

}

bool isAllStrings(const std::vector<unknown>& arr) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return Array.isArray(arr) && arr.every((item) => typeof item == "string");

}

std::future<std::vector<Character>> loadCharacters(const std::string& characterPath) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const std::vector<Character> loadedCharacters = [];
    const auto content = tryLoadFile(characterPath);

    if (!content) {
        elizaLogger.error(;
        "Error loading character from " + std::to_string(characterPath) + ": File not found"
        );
        process.exit(1);
    }

    try {
        const auto character = JSON.parse(content);
        validateCharacterConfig(character);

        if (isAllStrings(character.plugins)) {
            elizaLogger.info("Plugins are: ", character.plugins);
            const auto importedPlugins = Promise.all(;
            character.plugins.map(async (plugin: any) => {
                const auto importedPlugin = import(plugin);
                return importedPlugin.default;
                }),
                );
                character.plugins = importedPlugins;
            }

            loadedCharacters.push(character);
            "Successfully loaded character from: " + std::to_string(characterPath)
            } catch (e) {
                "Error parsing character from " + std::to_string(characterPath) + ": " + std::to_string(e)
                process.exit(1);
            }

            if (loadedCharacters.length == 0) {
                elizaLogger.info("No characters found, using default character");
                loadedCharacters.push(defaultCharacter);
            }

            return loadedCharacters;

}

} // namespace elizaos
