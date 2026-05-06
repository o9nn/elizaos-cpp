#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/cli/src/commands/start/utils/loader.h"

any tryLoadFile(string filePath)
{
    return serverTryLoadFile(filePath);
};


std::shared_ptr<Promise<array<std::shared_ptr<Character>>>> loadCharactersFromUrl(string url)
{
    return serverLoadCharactersFromUrl(url);
};


std::shared_ptr<Promise<std::shared_ptr<Character>>> jsonToCharacter(any character)
{
    return serverJsonToCharacter(character);
};


std::shared_ptr<Promise<std::shared_ptr<Character>>> loadCharacter(string filePath)
{
    return serverLoadCharacter(filePath);
};


std::shared_ptr<Promise<std::shared_ptr<Character>>> loadCharacterTryPath(string characterPath)
{
    return serverLoadCharacterTryPath(characterPath);
};


std::shared_ptr<Promise<array<std::shared_ptr<Character>>>> loadCharacters(string charactersArg)
{
    auto loadedCharacters = std::async([=]() { serverLoadCharacters(charactersArg); });
    if (loadedCharacters->length == 0) {
        logger->info(std::string("No characters found, using default character"));
        return array<any>{ defaultCharacter };
    }
    return loadedCharacters;
};


std::function<any()> hasValidRemoteUrls = [=]() mutable
{
    return serverHasValidRemoteUrls();
};

void Main(void)
{
}

MAIN
