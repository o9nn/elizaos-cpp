#include "loader.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string tryLoadFile(const std::string& filePath) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Use synchronous module loading to maintain backward compatibility
    const auto serverModule = loadModuleSync('@elizaos/server');
    return serverModule.tryLoadFile(filePath);

}

std::future<std::string> tryLoadFileAsync(const std::string& filePath) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Since this is a deprecated function delegating to server,
    // we need to load the server module asynchronously
    const auto serverModule = loadModule('@elizaos/server');
    return serverModule.tryLoadFile(filePath);

}

std::future<std::vector<Character>> loadCharactersFromUrl(const std::string& url) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto serverModule = loadModule('@elizaos/server');
    return serverModule.loadCharactersFromUrl(url);

}

std::future<Character> jsonToCharacter(unknown character) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto serverModule = loadModule('@elizaos/server');
    return serverModule.jsonToCharacter(character);

}

std::future<Character> loadCharacter(const std::string& filePath) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto serverModule = loadModule('@elizaos/server');
    return serverModule.loadCharacter(filePath);

}

std::future<Character> loadCharacterTryPath(const std::string& characterPath) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto serverModule = loadModule('@elizaos/server');
    return serverModule.loadCharacterTryPath(characterPath);

}

bool hasValidRemoteUrls() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Use synchronous module loading to maintain backward compatibility
    const auto serverModule = loadModuleSync('@elizaos/server');
    return serverModule.hasValidRemoteUrls();

}

std::future<bool> hasValidRemoteUrlsAsync() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Since this is a deprecated function delegating to server,
    // we need to load the server module asynchronously
    const auto serverModule = loadModule('@elizaos/server');
    return serverModule.hasValidRemoteUrls();

}

std::future<std::vector<Character>> loadCharacters(const std::string& charactersArg) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto serverModule = loadModule('@elizaos/server');
    // Delegate to server implementation for main loading logic
    const auto loadedCharacters = serverModule.loadCharacters(charactersArg);

    // CLI-specific behavior: fallback to default character if no characters found
    if (loadedCharacters.length == 0) {
        std::cout << 'No characters found << using default character' << std::endl;
        return [defaultCharacter];
    }

    return loadedCharacters;

}

} // namespace elizaos
