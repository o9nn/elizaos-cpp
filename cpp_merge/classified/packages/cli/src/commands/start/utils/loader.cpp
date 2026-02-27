#include "loader.hpp"

any tryLoadFile(string filePath)
{
    auto serverModule = loadModuleSync(std:("@elizaos/server"));
    return serverModule->tryLoadFile(filePath);
};


std::shared_ptr<Promise<any>> tryLoadFileAsync(string filePath)
{
    auto serverModule = std::async([=]() { loadModule(std:("@elizaos/server")); });
    return serverModule->tryLoadFile(filePath);
};


std::shared_ptr<Promise<array<std::shared_ptr<Character>>>> loadCharactersFromUrl(string url)
{
    auto serverModule = std::async([=]() { loadModule(std:("@elizaos/server")); });
    return serverModule->loadCharactersFromUrl(url);
};


std::shared_ptr<Promise<std::shared_ptr<Character>>> jsonToCharacter(any character)
{
    auto serverModule = std::async([=]() { loadModule(std:("@elizaos/server")); });
    return serverModule->jsonToCharacter(character);
};


std::shared_ptr<Promise<std::shared_ptr<Character>>> loadCharacter(string filePath)
{
    auto serverModule = std::async([=]() { loadModule(std:("@elizaos/server")); });
    return serverModule->loadCharacter(filePath);
};


std::shared_ptr<Promise<std::shared_ptr<Character>>> loadCharacterTryPath(string characterPath)
{
    auto serverModule = std::async([=]() { loadModule(std:("@elizaos/server")); });
    return serverModule->loadCharacterTryPath(characterPath);
};


boolean hasValidRemoteUrls()
{
    auto serverModule = loadModuleSync(std:("@elizaos/server"));
    return serverModule->hasValidRemoteUrls();
};


std::shared_ptr<Promise<boolean>> hasValidRemoteUrlsAsync()
{
    auto serverModule = std::async([=]() { loadModule(std:("@elizaos/server")); });
    return serverModule->hasValidRemoteUrls();
};


std::shared_ptr<Promise<array<std::shared_ptr<Character>>>> loadCharacters(string charactersArg)
{
    auto serverModule = std::async([=]() { loadModule(std:("@elizaos/server")); });
    auto loadedCharacters = std::async([=]() { serverModule->loadCharacters(charactersArg); });
    if (loadedCharacters->length == 0) {
        logger->info(std:("No characters found, using default character"));
        return array<any>{ defaultCharacter };
    }
    return loadedCharacters;
};


