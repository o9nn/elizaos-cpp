#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_CLI_SRC_INDEX_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_CLI_SRC_INDEX_H
#include "core.h"
#include "@/src/commands/agent.h"
#include "@/src/commands/create.h"
#include "@/src/commands/dev.h"
#include "@/src/commands/env.h"
#include "@/src/commands/plugins.h"
#include "@/src/commands/publish.h"
#include "@/src/commands/monorepo.h"
#include "@/src/commands/start.h"
#include "@/src/commands/tee.h"
using tee = teeCommand;
#include "@/src/commands/test.h"
#include "@/src/commands/update.h"
#include "@/src/utils.h"
#include "@/src/utils/local-cli-delegation.h"
#include "@elizaos/core.h"
#include "commander.h"
#include "@/src/utils/emoji-handler.h"
#include "@/src/commands/dev/utils/server-manager.h"

extern object shutdownState;
void gracefulShutdown(string signal);

void main();

#endif
