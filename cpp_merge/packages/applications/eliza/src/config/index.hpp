#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_STARTER_SRC_CONFIG_INDEX_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_STARTER_SRC_CONFIG_INDEX_H
#include "core.hpp"
// External dependency removed
#include "fs.hpp"
// Using alias removed (invalid transpilation)
#include "path.hpp"
// Using alias removed (invalid transpilation)
#include "yargs.hpp"
// Using alias removed (invalid transpilation)

object parseArguments();

std::shared_ptr<Promise<array<std::shared_ptr<Character>>>> loadCharacters(string charactersArg);

any getTokenForProvider(std::shared_ptr<ModelProviderName> provider, std::shared_ptr<Character> character);

#endif
