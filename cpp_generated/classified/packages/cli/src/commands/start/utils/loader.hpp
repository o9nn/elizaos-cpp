#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_CLI_SRC_COMMANDS_START_UTILS_LOADER_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_CLI_SRC_COMMANDS_START_UTILS_LOADER_H
#include "core.h"
#include "@elizaos/core.h"
#include "@/src/utils/module-loader.h"
#include "../../../characters/eliza.h"
using defaultCharacter = character;

any tryLoadFile(string filePath);

std::shared_ptr<Promise<any>> tryLoadFileAsync(string filePath);

std::shared_ptr<Promise<array<std::shared_ptr<Character>>>> loadCharactersFromUrl(string url);

std::shared_ptr<Promise<std::shared_ptr<Character>>> jsonToCharacter(any character);

std::shared_ptr<Promise<std::shared_ptr<Character>>> loadCharacter(string filePath);

std::shared_ptr<Promise<std::shared_ptr<Character>>> loadCharacterTryPath(string characterPath);

boolean hasValidRemoteUrls();

std::shared_ptr<Promise<boolean>> hasValidRemoteUrlsAsync();

std::shared_ptr<Promise<array<std::shared_ptr<Character>>>> loadCharacters(string charactersArg);

#endif
