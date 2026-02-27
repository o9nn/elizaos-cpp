#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA-STARTER_SRC_CLIENTS_INDEX_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA-STARTER_SRC_CLIENTS_INDEX_H
#include "core.h"
#include "@elizaos/client-auto.h"
#include "@elizaos/client-discord.h"
#include "@elizaos/client-telegram.h"
#include "@elizaos/client-twitter.h"
#include "@elizaos/core.h"

any initializeClients(std::shared_ptr<Character> character, std::shared_ptr<IAgentRuntime> runtime);

#endif
