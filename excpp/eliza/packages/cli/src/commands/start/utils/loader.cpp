#include "loader.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::optional<std::string> tryLoadFile(const std::string& filePath) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return serverTryLoadFile(filePath);

}

std::future<std::vector<Character>> loadCharactersFromUrl(const std::string& url) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return serverLoadCharactersFromUrl(url);

}

std::future<Character> jsonToCharacter(unknown character) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return serverJsonToCharacter(character);

}

std::future<Character> loadCharacter(const std::string& filePath) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return serverLoadCharacter(filePath);

}

std::future<Character> loadCharacterTryPath(const std::string& characterPath) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return serverLoadCharacterTryPath(characterPath);

}

std::future<std::vector<Character>> loadCharacters(const std::string& charactersArg) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Delegate to server implementation for main loading logic
    const auto loadedCharacters = serverLoadCharacters(charactersArg);

    // CLI-specific behavior: fallback to default character if no characters found
    if (loadedCharacters.length == 0) {
        std::cout << 'No characters found << using default character' << std::endl;
        return [defaultCharacter];
    }

    return loadedCharacters;

}

} // namespace elizaos
