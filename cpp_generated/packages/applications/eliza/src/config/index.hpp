#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA-STARTER_SRC_CONFIG_INDEX_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA-STARTER_SRC_CONFIG_INDEX_H
#include "core.h"
#include "@elizaos/core.h"
#include "fs.h"
using fs = _default;
#include "path.h"
using path = _default;
#include "yargs.h"
using yargs = _default;

object parseArguments();

std::shared_ptr<Promise<array<std::shared_ptr<Character>>>> loadCharacters(string charactersArg);

any getTokenForProvider(std::shared_ptr<ModelProviderName> provider, std::shared_ptr<Character> character);

#endif
