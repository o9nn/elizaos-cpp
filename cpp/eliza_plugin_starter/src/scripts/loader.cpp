#include "loader.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string tryLoadFile(const std::string& filePath) {
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

bool isAllStrings(const std::vector<std::any>& arr) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return Array.isArray(arr) && arr.every((item) => typeof item == "string");

}

std::future<std::vector<Character>> loadCharacters(const std::string& characterPath) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const std::vector<Character> loadedCharacters = [];
    const auto content = tryLoadFile(characterPath);

    if (!content) {
        elizaLogger.error(;
        "Error loading character from " + characterPath + ": File not found"
        );
        process.exit(1);
    }

    try {
        const auto character = /* JSON.parse */ content;
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

            loadedCharacters.push_back(character);
            "elizaLogger.info(" + "Successfully loaded character from: " + characterPath
            } catch (e) {
                "elizaLogger.error(" + "Error parsing character from " + characterPath + ": " + e
                process.exit(1);
            }

            if (loadedCharacters.length == 0) {
                elizaLogger.info("No characters found, using default character");
                loadedCharacters.push_back(defaultCharacter);
            }

            return loadedCharacters;

}

} // namespace elizaos
