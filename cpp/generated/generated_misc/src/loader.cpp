#include "eliza/packages/cli/src/commands/start/utils/loader.h"

std::any tryLoadFile(std::string filePath)
{
    return serverTryLoadFile(filePath);
};


std::shared_ptr<Promise<array<std::shared_ptr<Character>>>> loadCharactersFromUrl(std::string url)
{
    return serverLoadCharactersFromUrl(url);
};


std::shared_ptr<Promise<std::shared_ptr<Character>>> jsonToCharacter(std::any character)
{
    return serverJsonToCharacter(character);
};


std::shared_ptr<Promise<std::shared_ptr<Character>>> loadCharacter(std::string filePath)
{
    return serverLoadCharacter(filePath);
};


std::shared_ptr<Promise<std::shared_ptr<Character>>> loadCharacterTryPath(std::string characterPath)
{
    return serverLoadCharacterTryPath(characterPath);
};


std::shared_ptr<Promise<array<std::shared_ptr<Character>>>> loadCharacters(std::string charactersArg)
{
    auto loadedCharacters = std::async([=]() { serverLoadCharacters(charactersArg); });
    if (loadedCharacters->length == 0) {
        logger->info(std::string("No characters found, using default character"));
        return array<any>{ defaultCharacter };
    }
    return loadedCharacters;
};


std::function<std::any()> hasValidRemoteUrls = [=]() mutable
{
    return serverHasValidRemoteUrls();
};

void Main(void)
{
}

MAIN
