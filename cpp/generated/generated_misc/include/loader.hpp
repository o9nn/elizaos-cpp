#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLI_SRC_COMMANDS_START_UTILS_LOADER_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLI_SRC_COMMANDS_START_UTILS_LOADER_H
#include "core.h"
#include "@elizaos/core.h"
#include "@elizaos/server.h"
using serverTryLoadFile = tryLoadFile;
using serverLoadCharactersFromUrl = loadCharactersFromUrl;
using serverJsonToCharacter = jsonToCharacter;
using serverLoadCharacter = loadCharacter;
using serverLoadCharacterTryPath = loadCharacterTryPath;
using serverHasValidRemoteUrls = hasValidRemoteUrls;
using serverLoadCharacters = loadCharacters;
#include "../../../characters/eliza.h"
using defaultCharacter = character;

std::any tryLoadFile(std::string filePath);

std::shared_ptr<Promise<array<std::shared_ptr<Character>>>> loadCharactersFromUrl(std::string url);

std::shared_ptr<Promise<std::shared_ptr<Character>>> jsonToCharacter(std::any character);

std::shared_ptr<Promise<std::shared_ptr<Character>>> loadCharacter(std::string filePath);

std::shared_ptr<Promise<std::shared_ptr<Character>>> loadCharacterTryPath(std::string characterPath);

extern std::function<std::any()> hasValidRemoteUrls;
std::shared_ptr<Promise<array<std::shared_ptr<Character>>>> loadCharacters(std::string charactersArg);

#endif
