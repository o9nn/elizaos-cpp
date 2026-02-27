#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_CLI_SRC_COMMANDS_AGENT_UTILS_VALIDATION_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_CLI_SRC_COMMANDS_AGENT_UTILS_VALIDATION_H
#include "core.h"
#include "commander.h"
#include "zod.h"
#include "../../shared.h"
#include "../../shared.h"

extern any AgentBasicSchema;
extern any AgentsListResponseSchema;
std::shared_ptr<Promise<array<std::shared_ptr<AgentBasic>>>> getAgents(std::shared_ptr<OptionValues> opts);

std::shared_ptr<Promise<string>> resolveAgentId(string idOrNameOrIndex, std::shared_ptr<OptionValues> opts);

#endif
